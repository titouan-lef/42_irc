#pragma once

#include "ACommand.hpp"

struct Command;

class PingCmd : public ACommand
{
public:
	PingCmd();
	~PingCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};