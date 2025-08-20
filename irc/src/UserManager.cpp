#include "UserManager.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Server.hpp"
#include "Service.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "CommandManager.hpp"
#include "IrcException.hpp"
#include "ClientDisconnectedException.hpp"
#include "NoSuchNickException.hpp"

UserManager::UserManager() {}

UserManager& UserManager::getInstance()
{
	static UserManager instance;
	return instance;
}

/**
 * @brief Initializes the user manager by adding the server as the first user.
 *
 * @details The first pollfd in the list must be the server socket for the poll() function to work correctly.
 *
 * @param serverSocket The server's listening socket file descriptor.
 * @param serverAddr The server's socket address information.
 */
void UserManager::initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr)
{
	addNewUser(serverSocket, serverAddr);
}

void UserManager::addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr)
{
	for (std::size_t i = 0; i < pollfdList_.size(); i++) /** Protection for tester @todo remove before push */
	{
		if (pollfdList_.at(i).fd == clientSocket)
			throw std::runtime_error("fd already register");
	}

	Activity userActivity = {time(NULL), false};
	activityList_.push_back(userActivity);
	struct pollfd newPollfd = {clientSocket, POLLIN, 0}; // returned events = 0
	pollfdList_.push_back(newPollfd);
	userList_.push_back(User());
	addressList_.push_back(clientAddr);
	MsgServ::getInstance().addNewBuffer();
}

bool UserManager::isNickUsed(const std::string &nickname)
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
	{
		if (userList_.at(i).is(nickname))
			return true;
	}
	return Service::serviceExist(nickname);
}

/**
 * @brief Listens for incoming messages from connected clients and processes complete commands.
 *
 * @details Iterates through all connected clients, reads available data, and processes commands found in
 * their buffers. Handles client disconnection cleanly.
 */
void UserManager::listenClients()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
	{
		if (!hasDataToRead(i) || reiceveFromClient(i))
			continue;
		while (MsgServ::getInstance().isCommandReady(i))/** @todo why a "while" ? why "if (i >= userList_.size())" ? */
		{
			executeCommandsClient(i);
			if (i >= userList_.size()) /** @todo better management quit */
				break;
		}
	}
}

void UserManager::checkActivities()
{
	const time_t INACTIVITY_PING_LIMIT = 240;
	const time_t INACTIVITY_TIMEOUT = 360;

	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
	{
		Activity& userActivity = activityList_.at(i);
		time_t elapsed = time(NULL) - userActivity.timeLastActivity;
		if (elapsed > INACTIVITY_TIMEOUT)
		{
			closeConnection(i);
			continue;
		}
		if (userActivity.pingSend == true || elapsed <= INACTIVITY_PING_LIMIT)
			continue;
		int socket = pollfdList_.at(i).fd;
		MsgServ::getInstance().sendMessageReply(socket, socket, "PING", std::vector<std::string>(1, "ircserv"));
		userActivity.pingSend = true;
	}
}

/**
 * @brief Receives incoming messages from a client.
 *
 * Handles disconnections and receive-related errors gracefully.
 *
 * @param index Index of the user from whom data is expected.
 * 
 * @return 1 if the user has disconnected and the connection was closed, 0 otherwise.
 */
int UserManager::reiceveFromClient(const int index)
{
	try
	{
		MsgServ::getInstance().recvMsg(pollfdList_.at(index).fd, index);
	}
	catch(const ClientDisconnectedException& e)
	{
		closeConnection(index);
		return 1;
	}
	catch(const IrcException& e)
	{
		e.sendError(pollfdList_.at(index).fd);
	}
	return 0;
}

void UserManager::executeCommandsClient(const int index)
{
	std::string command = MsgServ::getInstance().getCommand(index);
	std::cout << "Executing command: " << command << " -> size: " << command.size() << std::endl << std::endl;
	try
	{
		CommandManager::getInstance().interpretData(command, pollfdList_.at(index).fd, userList_.at(index).getStatus());
	}
	catch(const IrcException& e)
	{
		e.sendError(pollfdList_.at(index).fd);
	}
}

/**
 * @param nickname The nickname of the user to find.
 * @return The socket descriptor of user.
 * @throw NoSuchNickException.
 */
int UserManager::findUserSocket(const std::string& nickname)
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
	{
		if (userList_.at(i).is(nickname))
			return pollfdList_.at(i).fd;
	}
	throw NoSuchNickException(nickname);
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return A reference of the user.
 * @throw NoSuchNickException.
 */
User& UserManager::findUser(const int userSocket)
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
	{
		const std::string nickname = User::getDefaultNickname();
		throw NoSuchNickException(nickname);
	}
	return userList_.at(index);
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The user nickname (or default user nickname if not found).
 */
const std::string UserManager::findUserNickname(const int userSocket) const
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
		return User::getDefaultNickname();
	return userList_.at(index).getNickname();
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The user name (or default user name if not found).
 */
const std::string UserManager::findUserName(const int userSocket) const
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
		return User::getDefaultUsername();
	return userList_.at(index).getUsername();
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The user prefix (or default user prefix if not found).
 */
const std::string UserManager::findUserPrefix(const int userSocket) const
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
		return User::getDefaultPrefix();
	return userList_.at(index).getPrefix();
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The index of socket user, 0 if is not found.
 */
std::size_t UserManager::findIndex(const int userSocket) const
{
	for (std::size_t i = 1; i < pollfdList_.size(); ++i)
	{
		if (pollfdList_.at(i).fd == userSocket)
			return i;
	}
	return 0;
}

void UserManager::closeAllConnection()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
		closeConnection(i);
	closeServer();
}

void UserManager::closeServer() { close(pollfdList_.at(0).fd); }

void UserManager::closeConnectionBySocket(const int userSocket)
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
	{
		std::cerr << "socket not found" << std::endl;
		return;
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
int UserManager::getNbUserEvent()
{
	return poll(pollfdList_.data(), pollfdList_.size(), 5000);
}

void UserManager::updateLastActivity(const int userSocket)
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
	{
		std::cerr << "socket not found" << std::endl;
		return;
	}
	if (activityList_.at(index).pingSend == false)
		activityList_.at(index).timeLastActivity = time(NULL);
}

void UserManager::validatePongResponse(const int userSocket)
{
	std::size_t index = findIndex(userSocket);
	if (index == 0)
	{
		std::cerr << "socket not found" << std::endl;
		return;
	}
	activityList_.at(index).pingSend = false;
}

bool UserManager::serverHasReiceveData() const { return (hasDataToRead(0)); }

/**
 * @brief Accepts and handles a new incoming client connection.
 * @throws std::runtime_error if accept or fcntl fails.
 */
void UserManager::checkNewConnections()
{
	if (UserManager::getInstance().serverHasReiceveData() == false)
		return;
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(pollfdList_.at(0).fd, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientSocket < 0)
		throw std::runtime_error("Error: accept failed, cannot connect client");
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) //set clientSocket non blocking on system calls
		throw std::runtime_error("Error: fcntl failed, cannot connect client");
	addNewUser(clientSocket, clientAddr);
	std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl << std::endl;
}

/**
 * @param index Index of the user socket in the pollfd list.
 * @return true if data is ready to read, false otherwise.
 */
bool UserManager::hasDataToRead(const int index) const
{
	return (pollfdList_.at(index).revents & POLLIN); /** @todo comment explanation */
}

void UserManager::closeConnection(const int index)
{
	int socket = pollfdList_.at(index).fd;
	ChanServ::getInstance().quit(socket);
	if (Server::getInstance().isUp())
		displayLogDisconnection(index);
	close(pollfdList_.at(index).fd);
	deleteUser(index);
}

void UserManager::displayLogDisconnection(const int index) const
{
	const User& user = userList_.at(index);
	if (user.getStatus() >= IDENTIFIED)
		std::cout << user.getNickname() + " disconnected" << std::endl;
	else
	{
		struct sockaddr_in addr = addressList_.at(index);
		std::cout << "Client " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " disconnected" << std::endl;
	}
}

void UserManager::deleteUser(const int index)
{
	if (index <= 0 || index >= (int)userList_.size())
		return;
	pollfdList_.erase(pollfdList_.begin() + index);
	userList_.erase(userList_.begin() + index);
	addressList_.erase(addressList_.begin() + index);
	activityList_.erase(activityList_.begin() + index);
	MsgServ::getInstance().removeBuffer(index);
}
