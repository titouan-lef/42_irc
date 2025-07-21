#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <ctime>

#include "User.hpp"

class CommandTester; /** @todo remove before push */
class PrivMsgTester; /** @todo remove before push */
class ModeCmdTester; /** @todo remove before push */
class TopicCmdTester; /** @todo remove before push */
class InviteCmdTester; /** @todo remove before push */
class PassCmdTester; /** @todo remove before push */
class NickCmdTester; /** @todo remove before push */
class UserCmdTester; /** @todo remove before push */
class JoinCmdTester; /** @todo remove before push */
class PartCmdTester; /** @todo remove before push */
class KickCmdTester; /** @todo remove before push */
class UserManagerTester; /** @todo remove before push */

class UserManager
{
public:
	static UserManager& getInstance();

	void initUserManager(const int serverSocket, const struct sockaddr_in& serverAddr);

	bool isServiceName(const std::string& name);
	void sendCommandToService(const int senderSocket, const std::string& name, const std::string& message);

	void addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr);
	bool isNickUsed(const std::string& nickname);
	void listenClients();
	void checkActivities();
	int reiceveFromClient(int i);
	void executeCommandsClient(int i);
	int findUserSocket(std::string nickname);
	void closeAllConnection();
	void closeConnectionBySocket(const int userSocket);

	bool serverReiceveData() const;
	void checkNewConnections();
	int getNbUserEvent() const;
	User& getUser(const int userSocket);
	const std::string getUserNickname(const int userSocket) const;
	const std::string getUserName(const int userSocket) const;
	const std::string getUserPrefix(const int userSocket) const;
	void updateLastActivity(const int userSocket);
	void validatePongResponse(const int userSocket);

private:

	struct Activity
	{
		time_t timeLastActivity;
		bool pingSend;
	};

	UserManager();
	UserManager(const UserManager& other);
	UserManager& operator=(const UserManager& other);

	bool hasDataToRead(const int index) const;
	void closeConnection(const int index);
	void deleteUser(const int index);
	size_t getIndex(const int userSocket);

	std::vector<User> userList_;
	std::vector<struct pollfd> pollfdList_;
	std::vector<struct sockaddr_in> addressList_;
	std::vector<Activity> lastActivities_;
	std::set<std::string> serviceNames_;

	friend class CommandTester; /** @todo remove before push */
	friend class PrivMsgTester; /** @todo remove before push */
	friend class ModeCmdTester; /** @todo remove before push */
	friend class TopicCmdTester; /** @todo remove before push */
	friend class InviteCmdTester; /** @todo remove before push */
	friend class PassCmdTester; /** @todo remove before push */
	friend class NickCmdTester; /** @todo remove before push */
	friend class UserCmdTester; /** @todo remove before push */
	friend class JoinCmdTester; /** @todo remove before push */
	friend class PartCmdTester; /** @todo remove before push */
	friend class KickCmdTester; /** @todo remove before push */
	friend class UserManagerTester; /** @todo remove before push */
};