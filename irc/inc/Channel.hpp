#pragma once

#include <vector>
#include <set>
#include "MsgServ.hpp"

class ChannelTester; /** @todo remove before push */
class ModeCmdTester; /** @todo remove before push */

class Channel
{
public:

	Channel(std::string name, const int socketModerator);
	// ~Channel();

	const std::string& getName() const; /** @todo getters inprivate */
	const std::string& getTopic() const;
	const std::string builtOptionString() const;
	bool isConnected(const int socketUser) const;
	bool isModerator(const int socketUser) const;
	bool isPasswordProtected() const;

	void connectUser(const int socketNewUser, const std::string& password = "");
	void disconnectUser(const int socketUser);

	void enableTopicRestriction();
	void disableTopicRestriction();
	void setTopic(const std::string& newTopic, const int socketUser);

	void setPassword(const std::string& newPassword);
	void disablePassword();

	void enableInviteRestriction();
	void disableInviteRestriction();
	void inviteUser(const int socketUser);

	void addNewModerator(const int socketNewOperator);
	void removeModerator(const int socketUser);

	int setLimitNbUsers(int nbUserMax);
	void disableLimitNbUsers();
	void sendMessageToAllUsers(const int senderSocket, const std::string msg); 

private:

	void sendNumericToAllUsers(const MsgServ::Command& cmd); 

	bool isInviteRestricted() const;
	bool isTopicRestricted() const;
	bool isLimitNbUserReached() const;

	const std::string name_;
	std::string topic_;
	std::string password_;
	int nbMaxUsers_;
	bool inviteRestricted_;
	bool topicRestricted_;
	std::set<int> socketInvitedUsers_;
	std::vector<int> socketConnectedUsers_;
	std::vector<int> socketModerators_;

	friend class ChannelTester ; /** @todo remove before push */
	friend class ModeCmdTester; /** @todo remove before push */
};