#pragma once

#include "Client.hpp"
#include "sys/socket.h"

class UserTester;

class User: public Client
{
public:

	User(struct sockaddr_in clientAddr);
	// ~User();

	bool is(std::string nickname);
	void validConnection();
	bool isConnected() const;
	bool isNickRegister() const;
	bool isUsernameRegister() const;

	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username); // ignore ??
	void setRealName(const std::string& realname);
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string getPrefix() const;
	struct sockaddr_in getAddress();

	bool isCommandReady() const;
	int completeMsg(const std::string& msg);
	std::string getCommand();

private:

	std::string nickname_;
	std::string username_;
	std::string realName_; // ignore ??
	bool isConnected_;

	struct sockaddr_in clientAddr_;
	// Role role_;

	std::string buffer_;

	friend class UserTester;
};