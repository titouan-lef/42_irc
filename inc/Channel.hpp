#pragma once

#include <string>
#include <vector>
#include <User.hpp>

class Channel
{
public:

	Channel(std::string name, User& moderator);
	// ~Channel();

	const std::string& getName();
	bool isInviteRestricted() const;
	bool isTopicRestricted() const;
	void connectUser(User& newUser);
	void disconnectUser(const std::string& nickname);
	bool isModerator(const std::string& nickname) const;
	void setTopic(const std::string& newTopic);
	void setPassword(const std::string& newPassword);
	void disablePassword();
	void addNewModerator(User& newOperator);
	void removeModerator(const std::string& nickname);
	void setLimitUsers(int nbUserMax);
	void disableLimitUsers();
	
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