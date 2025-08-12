#include "UserManager.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>

#include "Server.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "CommandManager.hpp"
#include "IrcException.hpp"
#include "ClientDisconnectedException.hpp"
#include "NoSuchNickException.hpp"

UserManager::UserManager()
{
	serviceNames_.insert("ChanServ");
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

UserManager& UserManager::getInstance()
{
	static UserManager instance;
	return instance;
}

void UserManager::addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr)
{
	for (std::size_t i = 0; i < pollfdList_.size(); i++) /** Protection for tester @todo remove before push */
	{
		if (pollfdList_.at(i).fd == clientSocket)
			throw std::runtime_error("fd already register");
	}

	Activity userActivity = {time(NULL), false};
	lastActivities_.push_back(userActivity);
	struct pollfd newPollfd = {clientSocket, POLLIN, 0};
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
	return isServiceName(nickname);
}

bool UserManager::isServiceName(const std::string &name) { return (serviceNames_.find(name) != serviceNames_.end()); }

void UserManager::sendCommandToService(const int senderSocket, const std::string &name, const std::string &message)
{
	if (name == "ChanServ")
		ChanServ::getInstance().receiveCommand(senderSocket, message);
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
		Activity& userActivity = lastActivities_.at(i);
		time_t elapsed = time(NULL) - userActivity.timeLastActivity;
		if (elapsed > INACTIVITY_PING_LIMIT && userActivity.pingSend == false)
		{
			int socket = pollfdList_.at(i).fd;
			MsgServ::getInstance().sendMessageReply(socket, socket, "PING", std::vector<std::string>(1, "ircserv"));
			userActivity.pingSend = true;
		}
		else if (elapsed > INACTIVITY_TIMEOUT)
			closeConnection(i);
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
		/** @todo not return 1 ? */
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
	ssize_t index = findIndex(userSocket);
	if (index <= 0)
	{
		const std::string nickname = User::getDefaultNickname();/** @todo check if is the good function (call before findUserNickname()) */
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
	ssize_t index = findIndex(userSocket);
	if (index <= 0)
		return User::getDefaultNickname(); /** @todo not better option */
	return userList_.at(index).getNickname();
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The user name (or default user name if not found).
 */
const std::string UserManager::findUserName(const int userSocket) const
{
	ssize_t index = findIndex(userSocket);
	if (index <= 0)
		return User::getDefaultUsername(); /** @todo not better option */
	return userList_.at(index).getUsername();
}

/**
 * @param userSocket The socket descriptor of the user.
 * @return The user prefix (or default user prefix if not found).
 */
const std::string UserManager::findUserPrefix(const int userSocket) const
{
	ssize_t index = findIndex(userSocket);
	if (index <= 0)
		return User::getDefaultPrefix(); /** @todo not better option */
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
	return 0; /** @todo not better option */
}

void UserManager::closeAllConnection()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
		closeConnection(i);
	closeServer();
}

void UserManager::closeConnectionBySocket(const int userSocket)
{
	int index = findIndex(userSocket);
	if (index <= 0)
	{
		std::cerr << "socket not found" << std::endl; /** @todo exception or just a log */
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
int UserManager::getNbUserEvent() const
{
	return poll((struct pollfd*)pollfdList_.data(), pollfdList_.size(), 5000);/** @todo cast */
}

void UserManager::updateLastActivity(const int userSocket)
{
	std::size_t index = findIndex(userSocket);
	if (lastActivities_.at(index).pingSend == false)/** @todo no security ? */
		lastActivities_.at(index).timeLastActivity = time(NULL);
}

void UserManager::validatePongResponse(const int userSocket)
{
	std::size_t index = findIndex(userSocket);/** @todo no security ? */
	lastActivities_.at(index).pingSend = false;
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
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
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
	return (pollfdList_.at(index).revents & POLLIN);
}

void UserManager::closeConnection(const int index)
{
	int socket = pollfdList_.at(index).fd;
	ChanServ::getInstance().quit(socket);
	if (Server::getInstance().isUp() && index != 0)/** @todo "index != 0" useless ? */
		displayLogDisconnection(index);
	close(pollfdList_.at(index).fd); /** @todo check with Lucien before this line is under else if */
	deleteUser(index);
}

void UserManager::displayLogDisconnection(const int index) const
{
	User user = userList_.at(index);
	if (user.getStatus() >= IDENTIFIED)
		std::cout << user.getNickname() + " disconnected" << std::endl;
	else
	{
		struct sockaddr_in addr = addressList_.at(index);
		std::cout << "Client " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " disconnected" << std::endl;
	}
}

void UserManager::closeServer() { close(pollfdList_.at(0).fd); }

void UserManager::deleteUser(const int index)
{
	if (index <= 0 || index >= static_cast<int>(userList_.size()))
		return; /** @todo or throw std::out_of_range("Invalid index in deleteUser"); */
	pollfdList_.erase(pollfdList_.begin() + index);
	userList_.erase(userList_.begin() + index);
	addressList_.erase(addressList_.begin() + index);
	lastActivities_.erase(lastActivities_.begin() + index);
	MsgServ::getInstance().removeBuffer(index);
}
