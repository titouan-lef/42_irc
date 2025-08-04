#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;
enum Status;

class PartCmd : public ACommand
{
public:
	PartCmd();
	~PartCmd() {}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;

private:
	void partChannelOfCmd(const Command& cmd, const int senderSocket) const;
	void partOneChannel(const char* message, const int senderSocket, const std::string& channelName) const;
};