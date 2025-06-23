#include "UserManager.hpp"
#include "MsgServ.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "CommandManager.hpp"
#include "NoSuchNickException.hpp"
#include "UnknowCmdException.hpp"

#include <unistd.h>

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
	User newUser(serverSocket, serverAddr);
	struct pollfd newPollfd = {serverSocket, POLLIN, 0};
	userList_.push_back(newUser);
	pollfdList_.push_back(newPollfd);
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
	User newUser(clientSocket, clientAddr);
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
 *       It currently sendback a test message to the client.
 */
void UserManager::listenClients()
{
	for (std::size_t i = userList_.size() - 1; i >= 1; --i)
	{
		if (!hasDataToRead(i))
			continue;
		try
		{
			std::string msg = MsgServ::recvMsg(pollfdList_[i].fd);
			std::cout << msg;

			CommandManager::getInstance().interpretData(msg, userList_[i]);
			std::string test = "testmsg";
			MsgServ::sendMsg(pollfdList_[i].fd, test);
			/**
			 * @todo
			 * receive message
			 * send to command manager
			 */
		}
		catch(const ClientDisconnectedException& e)
		{
			closeConnection(i);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
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
		if (userList_[i].is(nickname))
			return pollfdList_[i].fd;
	}
	return -1;
}

/**
 * @brief Closes all client connections.
 */
void UserManager::closeAllConnection()
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
		closeConnection(i);
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
 * @brief Checks if the server socket has data available to read.
 *
 * @return true if data is available on the server socket, false otherwise.
 */
bool UserManager::serverReiceveData() const
{
	return (getInstance().hasDataToRead(0));
}

/**
 * @brief Checks if there is data available to read on the user socket at the given index.
 *
 * @param index Index of the user socket in the pollfd list.
 * @return true if data is ready to read, false otherwise.
 */
bool UserManager::hasDataToRead(const int index) const
{
	return (pollfdList_[index].revents & POLLIN);
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
	User user = userList_[index];
	struct sockaddr_in addr = user.getAddress();
	std::string nickname = user.getNickname();
	if (!nickname.empty())
		std::cout << nickname + " disconnected" << std::endl;
	else
		std::cout << "Client " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << " disconnected" << std::endl;
	close(pollfdList_[index].fd);
	pollfdList_.erase(pollfdList_.begin() + index); //maybe not the best way
	userList_.erase(userList_.begin() + index); //maybe not the best way
	/**
	 * @todo
	 * remove pollfd
	 * remove user
	 */
}
