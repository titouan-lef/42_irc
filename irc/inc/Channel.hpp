#pragma once

#include <string>
#include <vector>
#include <User.hpp>

class Channel
{
public:

	Channel(std::string name, User& moderator);
	// ~Channel();

	const std::string& getName() const;
	bool isPasswordProtected() const;
	bool isInviteRestricted() const;
	bool isTopicRestricted() const;
	bool isLimitNbUserReached() const;
	bool isConnected(const std::string& nickname) const;
	bool isModerator(const std::string& nickname) const;

	void connectUser(User& newUser, const std::string& password = "");
	void disconnectUser(const std::string& nickname);
	void setTopic(const std::string& newTopic);
	void setPassword(const std::string& newPassword);
	void disablePassword();
	void addNewModerator(User& newOperator);
	void removeModerator(const std::string& nickname);
	void setLimitNbUsers(int nbUserMax);
	void disableLimitNbUsers();
	
private:
	
	void sendMsgToAllUsers(std::string& msg);

	const std::string name_;
	std::string topic_;
	std::string password_;
	int nbMaxUsers_;
	bool inviteRestricted_;
	bool topicRestricted_;
	std::vector<User> connectedUser_;
	std::vector<User> moderators_; // maybe just a vector of nicknames ??

};