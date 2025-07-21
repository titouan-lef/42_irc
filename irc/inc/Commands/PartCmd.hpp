#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmd(const Command& cmd) const;
	void partChannelOfCmd(const Command& cmd, const int senderSocket) const;
	void partOneChannel(const std::string& message, const int senderSocket, const std::string& channelName) const;
};