#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class PassCmd : public ACommand
{
public:
	PassCmd() {}
	~PassCmd() {}

	void execute(const Command& cmd, const int socket) const;
};