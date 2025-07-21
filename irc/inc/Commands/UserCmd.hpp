#pragma once

#include "ACommand.hpp"

#include <iostream>
#include <vector>

class User;
struct Command;

class UserCmd : public ACommand
{
public:
	UserCmd(){}
	~UserCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmd(const Command& cmd, const User& user) const;
	bool validParam(const std::vector<std::string>& param) const;
	bool validUsername(const std::string& username) const;
	void UpdateUser(User& user, const std::string& username, const std::string& realName) const;
	void sendHandshakeValidation(const int socket) const;
};