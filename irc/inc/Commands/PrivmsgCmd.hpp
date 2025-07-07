#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd(){}
	~PrivmsgCmd(){}

	void execute(const Command& cmd, const int socket) const;
};