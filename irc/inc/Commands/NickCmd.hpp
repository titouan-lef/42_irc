#pragma once

#include "ACommand.hpp"

#include <iostream>

class User;
struct Command;
enum Status;

class NickCmd : public ACommand
{
public:
	NickCmd();
	~NickCmd(){}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;

private:
	const std::string checkCmd(const Command& cmd) const;
	void checkNickname(const std::string& nickname) const;
	void checkNicknameSize(const std::string& nickname) const;
	void checkNicknameChar(const std::string& nickname) const;
	bool isAuthorizedChar(const unsigned char c) const;
	bool isAuthorizedFirstChar(const unsigned char c) const;
	void updateUser(User& user, const std::string& nickname, const Status& userStatus) const;
};