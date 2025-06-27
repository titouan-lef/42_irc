#pragma once

#include <string>
#include <vector>
#include <set>
#include <User.hpp> 
#include "MsgServ.hpp"

class ChannelTester; /** @todo remove before push */

class Channel
{
public:

	Channel(std::string name, User& moderator);
	// ~Channel();

	const std::string& getName() const; /** @todo getters inprivate */
	const std::string& getTopic() const;
	bool isConnected(const std::string& nickname) const;
	bool isModerator(const std::string& nickname) const;

	void connectUser(const User& newUser, const std::string& password = "");
	void disconnectUser(const std::string& nickname);

	void enableTopicRestriction();
	void disableTopicRestriction();
	void setTopic(const std::string& newTopic, const User& user);

	void setPassword(const std::string& newPassword);
	void disablePassword();

	void enableInviteRestriction();
	void disableInviteRestriction();
	void inviteUser(const std::string& nickname);

	void addNewModerator(const User& newOperator);
	void removeModerator(const std::string& nickname);

	int setLimitNbUsers(int nbUserMax);
	void disableLimitNbUsers();

private:

	void sendMessageToAllUsers(const MsgServ::Command& cmd); 
	void sendNumericToAllUsers(const MsgServ::Command& cmd); 

	bool isPasswordProtected() const;
	bool isInviteRestricted() const;
	bool isTopicRestricted() const;
	bool isLimitNbUserReached() const;

	const std::string name_;
	std::string topic_;
	std::string password_;
	int nbMaxUsers_;
	bool inviteRestricted_;
	bool topicRestricted_;
	std::set<std::string> invitedUsers_;
	std::vector<User> connectedUser_;
	std::vector<User> moderators_;

	friend class ChannelTester ;	/** @todo remove before push */
};