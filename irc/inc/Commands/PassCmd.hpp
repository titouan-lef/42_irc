#pragma once

#include "ACommand.hpp"

struct Command;

class PassCmd : public ACommand
{
public:
	PassCmd() {}
	~PassCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;
};