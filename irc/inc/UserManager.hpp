#pragma once

#include <iostream>
#include <vector>
#include <poll.h>

#include "User.hpp"

class UserManager
{
public:
	static UserManager& getInstance();

	void initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr);

	void addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr);
	void listenClients();
	int findUserSocket(std::string nickname);
	void closeAllConnection();

	bool serverReiceveData() const;
	int getNbUserEvent() const;

private:
	UserManager();
	UserManager(const UserManager& other);
	UserManager& operator=(const UserManager& other);

	bool hasDataToRead(const int index) const;
	void closeConnection(const int index);

	std::vector<User> userList_;
	std::vector<struct pollfd> pollfdList_;
};