#include "UserManager.hpp"
#include "MsgServ.hpp"
#include "ClientDisconnectedException.hpp"
#include "CommandManager.hpp"
#include "NoSuchNickException.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "UnknowCmdException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "AlreadyRegistredException.hpp"

#include <unistd.h>
#include <fcntl.h>

UserManager::UserManager(): defaultNickname_("*"), defaultPrefix_("*!*@localhost") {}

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
}

/**
 * @brief Listens for incoming messages from connected clients.
 *
 * Handles client disconnection and receive errors gracefully.
 *
 * @note Maybe it would be better to return found message and handle it somewhere else??
 *	   It currently sendback a test message to the client.
 */
void UserManager::listenClients()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
	{
		if (hasDataToRead(i))
		{
			reiceveFromClient(i);
		}
		while (userList_.at(i).isCommandReady())
		{
			executeCommandsClient(i);
		}
	}
}

void UserManager::reiceveFromClient(int i)
{
	try
	{
		std::string msg = MsgServ::recvMsg(pollfdList_.at(i).fd, userList_.at(i).getNickname());
		std::cout << "Received message: " << msg << "	size: " << msg.size() << std::endl;
		if (userList_.at(i).completeMsg(msg))
			std::cout << "Error: msg too long." << std::endl;// throw MessageToLongException();
	}
	catch(const ClientDisconnectedException& e)
	{
		closeConnection(i);
	}
	catch(const IrcException& e)
	{
		e.sendError();
	}
}

void UserManager::executeCommandsClient(int i)
{
	try
	{
		std::string command = userList_.at(i).getCommand();
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

User& UserManager::getUser(const int socket)
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i);
	}
	return userList_.at(0);/** @todo not better option */
}

const std::string& UserManager::getUserNickname(const int socket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i).getNickname();
	}
	return defaultNickname_; /** @todo not better option */
}

const std::string UserManager::getUserPrefix(const int socket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == socket)
			return userList_.at(i).getPrefix();
	}
	return defaultPrefix_; /** @todo not better option */
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

void UserManager::deleteUser(const int index)
{
	pollfdList_.erase(pollfdList_.begin() + index); //maybe not the best way
	userList_.erase(userList_.begin() + index); //maybe not the best way
}
