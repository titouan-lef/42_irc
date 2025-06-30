#pragma once

#include "Client.hpp"
#include "sys/socket.h"

class UserTester;

class User: public Client
{
public:

	User(struct sockaddr_in clientAddr);
	// ~User();

	bool is(const std::string& nickname) const;
	bool isConnected() const;
	bool isNickRegister() const;
	bool isUsernameRegister() const;
	bool isCommandReady() const;
	void validConnection();
	int completeMsg(const std::string& msg);
	
	static const std::string getDefaultNickname();
	static const std::string getDefaultPrefix();
	const std::string& getNickname() const;
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username); /** @todo ignore ?? */
	void setRealName(const std::string& realname);
	const std::string getPrefix() const;
	struct sockaddr_in getAddress() const;
	std::string getCommand();

private:
	const std::string getDefaultUsername() const;
	const std::string& getUsername() const;

	std::string nickname_;
	std::string username_;
	std::string realName_; /** @todo ignore ?? */
	bool isConnected_;
	struct sockaddr_in clientAddr_;
	std::string buffer_;

	friend class UserTester;
};