#pragma once

#include "ACommand.hpp"

struct Command;

class InviteCmd : public ACommand
{
public:
	InviteCmd(){}
	~InviteCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};