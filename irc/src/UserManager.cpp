#include "UserManager.hpp"
#include "MsgServ.hpp"
#include "CommandManager.hpp"
#include "IrcException.hpp"
#include "ClientDisconnectedException.hpp"

#include <unistd.h>
#include <fcntl.h>

UserManager::UserManager() {}

/**
 * @brief Initializes the user manager by adding the server as the first user.
 *
 * The first pollfd in the list must be the server socket for the poll() function to work correctly.
 *
 * @param serverSocket The server's listening socket file descriptor.
 * @param serverAddr The server's socket address information.
 */
void UserManager::initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr)
{
	addNewUser(serverSocket, serverAddr);
}

/**
 * @brief Returns the singleton instance of UserManager.
 *
 * This method ensures that only one instance of UserManager exists
 * throughout the program lifetime.
 *
 * @return Reference to the single ChanServ instance.
 */
UserManager& UserManager::getInstance()
{
	static UserManager instance;
	return instance;
}

/**
 * @brief Adds a new user to the manager by creating a User and pollfd entry.
 *
 * @param clientSocket The socket descriptor of the new client.
 * @param clientAddr The socket address information of the new client.
 */
void UserManager::addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr)
{
	User newUser(clientAddr);
	struct pollfd newPollfd = {clientSocket, POLLIN, 0};
	userList_.push_back(newUser);
	pollfdList_.push_back(newPollfd);
	buffers_.push_back("");
}

/**
 * @brief Listens for incoming messages from connected clients and processes complete commands.
 *
 * Iterates through all connected clients, reads available data, and processes commands found in
 * their buffers. Handles client disconnection cleanly.
 */
void UserManager::listenClients()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
	{
		if (!hasDataToRead(i) || reiceveFromClient(i))
			return;
		while (isCommandReady(i))
		{
			executeCommandsClient(i);
			if (i >= userList_.size()) /** @todo betgter management quit */
				break;
		}
	}
}

/**
 * @brief Receives incoming messages from a client.
 *
 * Handles disconnections and receive-related errors gracefully.
 *
 * @param i Index of the user from whom data is expected.
 * 
 * @return 1 if the user has disconnected and the connection was closed, 0 otherwise.
 */
int UserManager::reiceveFromClient(int i)
{
	try
	{
		std::string msg = MsgServ::recvMsg(pollfdList_.at(i).fd, userList_.at(i).getNickname());
		std::cout << "Received message: " << msg << "	size: " << msg.size() << std::endl;
		if (completeMsg(i, msg))
			std::cout << "Error: msg too long." << std::endl;/** @todo throw MessageToLongException(); */
	}
	catch(const ClientDisconnectedException& e)
	{
		closeConnection(i);
		return 1;
	}
	catch(const IrcException& e)
	{
		e.sendError();
	}
	return 0;
}

/**
 * @brief Executes all complete commands contained in the buffer of the specified user.
 *
 * @param i Index of the user in userList_ and pollfdList_.
 */
void UserManager::executeCommandsClient(int i)
{
	try
	{
		std::string command = getCommand(i);
		std::cout << "Executing command: " << command << " -> size: " << command.size() << std::endl;
		if (userList_.at(i).isUsernameRegister())
			CommandManager::getInstance().interpretData(command, pollfdList_.at(i).fd);
		else
			CommandManager::getInstance().registerClient(command, pollfdList_.at(i).fd);
	}
	catch(const IrcException& e)
	{
		e.sendError();
	}
}

/**
 * @brief Finds the socket descriptor associated with a user by their nickname.
 *
 * @param nickname The nickname of the user to find.
 * 
 * @return The socket descriptor if found, or -1 if no matching user exists.
 * 
 * @note Unused for now...
 */
int UserManager::findUserSocket(std::string nickname)
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
	{
		if (userList_.at(i).is(nickname))
			return pollfdList_.at(i).fd;
	}
	return -1;
}

/**
 * @brief Closes all client connections.
 */
void UserManager::closeAllConnection()
{
	for (std::size_t i = userList_.size() - 1; i > 0; --i)/** @todo see to one instruction but size_t >= 0 (/!\ --i) */
		closeConnection(i);
	closeConnection(0);
}
/**
 * @brief Closes the user connection at the given socket.
 *
 * Logs the disconnection event using the user's nickname if available,
 * otherwise logs the client IP and port.
 *
 * @param socket The Socket of the connection/user to close.
 */
void UserManager::closeConnectionBySocket(const int socket)
{
	int index = getIndex(socket);
	if (index == -1)
	{
		std::cerr << "socket not found" << std::endl; /** @todo exception or just a log */
		return ;
	}
	closeConnection(index);
}

/**
 * @brief Waits for events on the monitored sockets.
 *
 * Uses the poll system call to wait up to 5 seconds for socket events.
 *
 * @return The number of file descriptors with events, 0 on timeout, or -1 on error.
 */
int UserManager::getNbUserEvent() const
{
	return poll((struct pollfd*)pollfdList_.data(), pollfdList_.size(), 5000);/** @todo cast */
}

/**
 * @brief Retrieves a reference to the user associated with the given socket.
 *
 * @param socket The socket descriptor of the user.
 * 
 * @return A reference to the corresponding User. If not found, returns the default user at index 0.
 * 
 * @note Returning userList_[0] is ugly: Maybe throwing an exception or returning a pointer instead.
 */
User& UserManager::getUser(const int socket)
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i);
	}
	return userList_.at(0);/** @todo not better option */
}

/**
 * @brief Retrieves the nickname of the user associated with the given socket.
 *
 * @param socket The socket descriptor of the user.
 * 
 * @return A reference to the nickname string. Returns a default nickname if not found.
 */
const std::string UserManager::getUserNickname(const int socket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i).getNickname();
	}
	return User::getDefaultNickname(); /** @todo not better option */
}

/**
 * @brief Retrieves the full IRC prefix (nickname!user@host) for the given socket.
 *
 * @param socket The socket descriptor of the user.
 * 
 * @return The IRC prefix string. Returns a default prefix if the user is not found.
 */
const std::string UserManager::getUserPrefix(const int socket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i).getPrefix();
	}
	return User::getDefaultPrefix(); /** @todo not better option */
}

/**
 * @brief Checks if the server socket has data available to read.
 *
 * @return true if data is available on the server socket, false otherwise.
 */
bool UserManager::serverReiceveData() const
{
	return (getInstance().hasDataToRead(0));
}

/**
 * @brief Accepts and handles a new incoming client connection.
 *
 * @throws std::runtime_error if accept or fcntl fails.
 */
void UserManager::checkNewConnections()
{
	if (!UserManager::getInstance().serverReiceveData())
		return;
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(pollfdList_.at(0).fd, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientSocket < 0)
		throw std::runtime_error("Error: accept failed");
	if(fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl failed, cannot connect client");
	addNewUser(clientSocket, clientAddr);
	std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
}

/**
 * @brief Checks if there is data available to read on the user socket at the given index.
 *
 * @param index Index of the user socket in the pollfd list.
 * @return true if data is ready to read, false otherwise.
 */
bool UserManager::hasDataToRead(const int index) const
{
	return (pollfdList_.at(index).revents & POLLIN);
}

/**
 * @brief Closes the user connection at the given index.
 *
 * Logs the disconnection event using the user's nickname if available,
 * otherwise logs the client IP and port.
 *
 * @param index The index of the connection/user to close.
 */
void UserManager::closeConnection(const int index)
{
	User user = userList_.at(index);
	if (user.isNickRegister())
		std::cout << user.getNickname() + " disconnected" << std::endl;
	else if (index != 0)
	{
		struct sockaddr_in addr = user.getAddress();
		std::cout << "Client " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " disconnected" << std::endl;
	}
	close(pollfdList_.at(index).fd);
	deleteUser(index);
}

/**
 * @brief Removes a user from the server's tracking lists by index.
 *
 * Erases the user and their corresponding pollfd entry from their respective vectors.
 * 
 * @param index The index of the user to delete (must be valid).
 */
void UserManager::deleteUser(const int index)
{
	if (index <= 0 || index >= static_cast<int>(userList_.size()))
		return; // or throw std::out_of_range("Invalid index in deleteUser");
	pollfdList_.erase(pollfdList_.begin() + index);
	userList_.erase(userList_.begin() + index);
	buffers_.erase(buffers_.begin() + index);
}

/**
 * @brief Check if the this user has received a complete command ready to execute.
 *
 * @return true if the buffer contain at least one complete command, 0 otherwise.
 */
bool UserManager::isCommandReady(int i) const
{
	std::string buffer = buffers_.at(i);
	std::cout << "Actual buffer_: " << buffer << " -> size: " << buffer.size() << std::endl;
	return buffer.find("\r\n") != std::string::npos || buffer.find("\n") != std::string::npos;
}

/**
 * @brief Add the reiceved message to the user's buffer.
 * 
 * If the buffer was containing the beggin of a command, if will complete the command.
 * If the buffer was empty, it will simply sto the received message in the buffer.
 *
 * @return 1 if the total length of the current command is longer that 512, 0 otherwize.
 * 
 * @note You must the return value of this funciton and throw MessageToLongException() if true.
 */
int UserManager::completeMsg(int i, const std::string &msg)
{
	std::string& buffer = buffers_.at(i);
	buffer += msg;
	if (buffer.size() > 512)
	{
		buffer.clear();
		return 1;
	}
	return 0;
}

/**
 * @brief Extracts and returns the next complete command from the buffer.
 * 
 * The function removes the command from the internal buffer after extraction.
 * 
 * @return The next command if available, or an empty string if no complete command is found.
 * 
 * @note According to the IRC protocol, "\r\n" is the standard line ending.
 *       If only "\n" is found (non-standard), it is still accepted for flexibility (netcat).
 */
std::string UserManager::getCommand(int i)
{
	std::string& buffer = buffers_.at(i);
	size_t pos = buffer.find("\r\n");
	if (pos == std::string::npos)
	{
		pos = buffer.find("\n");
		if (pos == std::string::npos)
			return "";
		std::string command = buffer.substr(0, pos);
		buffers_[i] = buffer.substr(pos + 1);
		return command;
	}
	std::string command = buffer.substr(0, pos);
	buffer = buffer.substr(pos + 2);
	return command;
}

int UserManager::getIndex(const int socket)
{
	for (size_t index = 0; index < userList_.size(); ++index)
	{
		if (pollfdList_[index].fd == socket) 
			return static_cast<int>(index);
	}
	return (-1);
}
