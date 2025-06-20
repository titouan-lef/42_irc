#include "UserManager.hpp"

#include <unistd.h>

UserManager::UserManager() {}

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
	for (std::size_t i = 0; i < userList_.size(); ++i)
	{
		if (!hasDataToRead(i))
			continue;
		/**
		 * @todo
		 * receive message
		 * send to command manager
		 */
	}
}

int UserManager::findUserSocket(std::string nickname)
{
	for (std::size_t i = 0; i < userList_.size(); ++i)
	{
		if (userList_[i].is(nickname))
			return pollfdList_[i].fd;
	}
	return -1;
}

void UserManager::closeAllConnection()
{
	for (std::size_t i = 0; i < userList_.size(); ++i)
		closeConnection(i);
}

int UserManager::getNbUserEvent() const
{
	return poll((struct pollfd*)pollfdList_.data(), pollfdList_.size(), 5000);// todo cast
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
