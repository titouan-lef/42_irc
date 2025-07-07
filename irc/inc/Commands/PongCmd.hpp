#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class PongCmd : public ACommand
{
public:
	PongCmd(){}
	~PongCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};