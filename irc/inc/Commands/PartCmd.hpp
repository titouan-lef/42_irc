#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class PartCmd : public ACommand
{
public:
	PartCmd();
	~PartCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void partChannelOfCmd(const Command& cmd, const int senderSocket) const;
	void partOneChannel(const char* message, const int senderSocket, const std::string& channelName) const;
};