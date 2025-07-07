#pragma once

#include "command.hpp"

class ACommand
{
public:
	ACommand(){}
	virtual ~ACommand(){}

	virtual void execute(const Command& cmd, const int socket) const = 0;
};