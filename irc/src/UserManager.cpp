#include "UserManager.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "CommandManager.hpp"
#include "IrcException.hpp"
#include "ClientDisconnectedException.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

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
	User newUser;
	struct pollfd newPollfd = {clientSocket, POLLIN, 0};
	userList_.push_back(newUser);
	pollfdList_.push_back(newPollfd);
	addressList_.push_back(clientAddr);
	MsgServ::getInstance().addNewBuffer();
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
			continue;
		while (MsgServ::getInstance().isCommandReady(i))
		{
			executeCommandsClient(i);
			if (i >= userList_.size()) /** @todo better management quit */
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
		MsgServ::getInstance().recvMsg(pollfdList_.at(i).fd, i);
	}
	catch(const ClientDisconnectedException& e)
	{
		closeConnection(i);
		return 1;
	}
	catch(const IrcException& e)
	{
		e.sendError(pollfdList_.at(i).fd);
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
		std::string command = MsgServ::getInstance().getCommand(i);
		std::cout << "Executing command: " << command << " -> size: " << command.size() << std::endl << std::endl;
		if (userList_.at(i).isUsernameRegister())
			CommandManager::getInstance().interpretData(command, pollfdList_.at(i).fd);
		else
			CommandManager::getInstance().registerClient(command, pollfdList_.at(i).fd);
	}
	catch(const IrcException& e)
	{
		e.sendError(pollfdList_.at(i).fd);
	}
}

/**
 * @brief Finds the socket descriptor associated with a user by their nickname.
 *
 * @param nickname The nickname of the user to find.
 * 
 * @return The socket descriptor if found, or -1 if no matching user exists.
 * 
 * @note You must throw NoSuchNickException if return -1.
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
void UserManager::closeConnectionBySocket(const int userSocket)
{
	int index = getIndex(userSocket);
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
User& UserManager::getUser(const int userSocket)
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == userSocket)
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
const std::string UserManager::getUserNickname(const int userSocket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == userSocket)
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
const std::string UserManager::getUserPrefix(const int userSocket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == userSocket)
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
		throw std::runtime_error("Error: accept failed, cannot connect client");
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl failed, cannot connect client");
	addNewUser(clientSocket, clientAddr);
	std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl << std::endl;
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
	int socket = pollfdList_.at(index).fd;
	ChanServ::getInstance().quitAllChannels(socket);
	User user = userList_.at(index);
	if (user.isNickRegister())
		std::cout << user.getNickname() + " disconnected" << std::endl;
	else if (index != 0)
	{
		struct sockaddr_in addr = addressList_.at(index);
		std::cout << "Client " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " disconnected" << std::endl;
	}
	addressList_.erase(addressList_.begin() + index);
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
	MsgServ::getInstance().removeBuffer(index);
}


/**
 * @brief Find the index of socket user
 * 
 * @param index The index of socket user, -1 if is not found;
 */
int UserManager::getIndex(const int userSocket)
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == userSocket)
			return i;
	}
	return -1; /** @todo not better option */
}