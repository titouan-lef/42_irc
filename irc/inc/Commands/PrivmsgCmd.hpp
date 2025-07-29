#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd(){}
	~PrivmsgCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmdParam(const Command& cmd) const;
	void checkNbTarget(const std::size_t nbTarget, const std::string &target) const;
	void sendToTarget(const Command& cmd, const int senderSocket, const std::string& target) const;
	bool isMsgToChannel(const std::string& target) const;
	void sendMsgToChannel(const Command &cmd, const int senderSocket, const std::string& channelName) const;
	bool isMsgToService(const std::string& target) const;
	void sendMsgToService(const std::string& message, const int senderSocket, const std::string& serviceName) const;
	void sendMsgToUser(const Command &cmd, const int senderSocket, const std::string& nickname) const;
};