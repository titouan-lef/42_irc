#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;
enum Status;

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd();
	~PrivmsgCmd(){}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;

private:
	void checkCmd(const Command& cmd) const;
	void checkNbTarget(const std::size_t nbTarget, const std::string &target) const;
	void sendToTarget(const Command& cmd, const int senderSocket, const std::string& target) const;
	bool isMsgToChannel(const std::string& target) const;
	void sendMsgToChannel(const Command &cmd, const int senderSocket, const std::string& channelName) const;
	void sendMsgToUser(const Command &cmd, const int senderSocket, const std::string& nickname) const;
};