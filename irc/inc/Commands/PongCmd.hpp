#pragma once

#include "ACommand.hpp"

struct Command;

class PongCmd : public ACommand
{
public:
	PongCmd(){}
	~PongCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};