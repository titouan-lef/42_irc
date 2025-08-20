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

	void addNewUser(const int clientSocket, const struct sockaddr_in& clientAddr);
	bool isNickUsed(const std::string& nickname);
	void listenClients();
	void checkActivities();
	int findUserSocket(const std::string& nickname);
	User& findUser(const int userSocket);
	const std::string findUserNickname(const int userSocket) const;
	const std::string findUserName(const int userSocket) const;
	const std::string findUserPrefix(const int userSocket) const;
	void closeAllConnection();
	void closeConnectionBySocket(const int userSocket);

	void checkNewConnections();
	int getNbUserEvent();
	void updateLastActivity(const int userSocket);
	void validatePongResponse(const int userSocket);

private:
	struct Activity
	{
		time_t timeLastActivity;
		bool pingSend;
	};

	UserManager();

	int reiceveFromClient(const int index);
	void executeCommandsClient(const int index);
	bool serverHasReiceveData() const;
	bool hasDataToRead(const int index) const;
	bool userIsStillConnected(const std::size_t currentNbUser) const;
	void closeConnection(const int index);
	void displayLogDisconnection(const int index) const;
	void closeServer();
	void deleteUser(const int index);
	std::size_t findIndex(const int userSocket) const;

	std::vector<User> userList_;
	std::vector<struct pollfd> pollfdList_;
	std::vector<struct sockaddr_in> addressList_;
	std::vector<Activity> activityList_;

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