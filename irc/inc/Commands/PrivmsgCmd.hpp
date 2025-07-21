#pragma once

#include "ACommand.hpp"

struct Command;

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd(){}
	~PrivmsgCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};