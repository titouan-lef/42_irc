#include "UserManager.hpp"
#include "MsgServ.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"

#include <unistd.h>

UserManager::UserManager() {}

void UserManager::initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr)
{
	User newUser(serverSocket, serverAddr);
	struct pollfd newPollfd = {serverSocket, POLLIN, 0};
	userList_.push_back(newUser);
	pollfdList_.push_back(newPollfd);
}

UserManager& UserManager::getInstance()
{
	static UserManager instance;
	return instance;
}

void UserManager::addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr)
{
	User newUser(clientSocket, clientAddr);
	struct pollfd newPollfd = {clientSocket, POLLIN, 0};
	userList_.push_back(newUser);
	pollfdList_.push_back(newPollfd);
}

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
			getInstance().closeConnection(i);
		}
		catch(const ReiceveException& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

int UserManager::findUserSocket(std::string nickname)
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
	{
		if (userList_[i].is(nickname))
			return pollfdList_[i].fd;
	}
	return -1;
}

void UserManager::closeAllConnection()
{
	for (std::size_t i = 1; i < userList_.size(); ++i)
		closeConnection(i);
}

int UserManager::getNbUserEvent() const
{
	return poll((struct pollfd*)pollfdList_.data(), pollfdList_.size(), 5000);/** @todo cast */
}

bool UserManager::serverReiceveData() const
{
	return (getInstance().hasDataToRead(0));
}

bool UserManager::hasDataToRead(const int index) const
{
	return (pollfdList_[index].revents & POLLIN);
}

void UserManager::closeConnection(const int index)
{
	close(pollfdList_[index].fd);
	/**
	 * @todo
	 * remove pollfd
	 * remove user
	 */
}
