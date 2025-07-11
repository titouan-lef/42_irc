#pragma once

#include <iostream>
#include <vector>
#include <set>

#include "command.hpp"

class ChannelTester; /** @todo remove before push */
class ModeCmdTester; /** @todo remove before push */
class TopicCmdTester; /** @todo remove before push */

class Channel
{
public:
	Channel(std::string name, const int socketOperator);
	// ~Channel();

	void sendChannelSummary(const int socket) const;
	void sendActualTopic(const int socket) const;
	void sendActualUsers(const int socket) const;
	void sendActualModes(const int socket) const;
	void sendMessageToAllUsers(const int senderSocket, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &trailing) const;

	void connectUser(const int socketNewUser, const std::string& password = "");
	bool disconnectUser(const int socketUser);
	void inviteUser(int invitedSocket, int socketSender);

	void managePartMessage(const int socketUser, const std::string &message) const;
	void manageKickMessage(const int senderSocket, const std::string &username, const std::string &message) const;

	void addNewOperator(const int socketNewOperator);
	void removeOperator(const int socketUser);

	const std::string& getName() const; /** @todo getters inprivate */
	bool is(const std::string& name) const;

	bool isConnected(const int socketUser) const;
	void insertConnectedUsers(std::set<int>& list);
	bool isOperator(const int socketUser) const;

	bool isPasswordProtected() const;
	void disablePassword();
	void setPassword(const std::string& newPassword);

	void disableLimitNbUsers();
	int setLimitNbUsers(std::size_t nbUserMax);

	void enableInviteRestriction();
	void disableInviteRestriction();

	void enableTopicRestriction();
	void disableTopicRestriction();
	void setTopic(const std::string& newTopic, const int socketUser);

private:
	void manageJoinMessage(const int socketNewUser) const;
	void manageCmdMessage(const int socketUser, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &trailing) const;

	/** @todo void sendNumericToAllUsers(const Command& cmd); */
	const std::string builtUsersString() const;
	const std::string builtOptionString() const;

	const std::string sizeToStr(const std::size_t nb) const;

	void removeConnectedUser(const int socketUser);

	void eraseUser(std::vector<int>& socketArray, const int socketUser);
	std::vector<int>::iterator findUser(std::vector<int>& socketArray, const int socketUser) const;
	std::vector<int>::const_iterator findUser(const std::vector<int>& socketArray, const int socketUser) const;

	bool isFull() const;
	bool isInvited(const int socketNewUser) const;
	bool isInviteRestricted() const;
	bool isTopicRestricted() const;

	std::string name_;
	std::string topic_;
	std::string password_;
	std::size_t nbMaxUsers_;
	bool inviteRestricted_;
	bool topicRestricted_;
	std::set<int> socketInvitedUsers_;
	std::vector<int> socketConnectedUsers_;
	std::vector<int> socketOperators_;

	friend class ChannelTester ; /** @todo remove before push */
	friend class ModeCmdTester; /** @todo remove before push */
	friend class TopicCmdTester; /** @todo remove before push */
};
