#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class PingCmd : public ACommand
{
public:
	PingCmd(){}
	~PingCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};