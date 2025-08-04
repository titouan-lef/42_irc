#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;
enum Status;

class InviteCmd : public ACommand
{
public:
	InviteCmd();
	~InviteCmd(){}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;

private:
	void inviteUserInChannel(const Command& cmd, const int senderSocket, const std::string& channelName, const int invitedSocket) const;
};