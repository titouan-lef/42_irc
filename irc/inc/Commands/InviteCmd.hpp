#pragma once

#include "ACommand.hpp"

struct Command;

class InviteCmd : public ACommand
{
public:
	InviteCmd(){}
	~InviteCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	const std::string checkCmd(const Command& cmd) const;
	void inviteUserInChannel(const Command& cmd, const int senderSocket, const std::string& channelName, const int invitedSocket) const;
};