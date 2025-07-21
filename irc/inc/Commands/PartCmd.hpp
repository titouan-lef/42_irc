#pragma once

#include "ACommand.hpp"

struct Command;

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;

};