#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmd(const Command& cmd) const;
	void checkCmdArg(const Command& cmd, const std::size_t nbChannel, const std::size_t nbUser) const;
	const std::string buildMessage(const std::string& trailing, const int senderSocket) const;
	const std::string& getChannelName(const std::size_t userIndex, const std::vector<std::string>& channelList) const;
	void kickOneChannel(const int senderSocket, const std::string& channelName, const std::string& nickname, const std::string& message) const;
	void checkChannel(const int senderSocket, const std::string& channelName) const;
};