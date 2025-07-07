#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class InviteCmd : public ACommand
{
public:
	InviteCmd(){}
	~InviteCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};