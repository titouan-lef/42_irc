#pragma once

#include <iostream>
#include <vector>
#include <poll.h>

#include "User.hpp"

class CommandTester; /** @todo remove before push */

class UserManager
{
public:
	static UserManager& getInstance();

	void initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr);

	void addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr);
	void listenClients();
	int reiceveFromClient(int i);
	void executeCommandsClient(int i);
	int findUserSocket(std::string nickname);
	void closeAllConnection();
	void closeConnectionBySocket(const int socket);

	bool serverReiceveData() const;
	void checkNewConnections();
	int getNbUserEvent() const;
	User& getUser(const int socket);
	const std::string getUserNickname(const int socket) const;
	const std::string getUserPrefix(const int socket) const;

private:
	UserManager();
	UserManager(const UserManager& other);
	UserManager& operator=(const UserManager& other);

	bool hasDataToRead(const int index) const;
	void closeConnection(const int index);
	void deleteUser(const int index);
	bool isCommandReady(int i) const;
	int completeMsg(int i, const std::string &msg);
	std::string getCommand(int i);
	int getIndex(const int socket);

	std::vector<User> userList_;
	std::vector<struct pollfd> pollfdList_;
	std::vector<std::string> buffers_;

	friend class CommandTester; /** @todo remove before push */
};