#pragma once

#include "ACommand.hpp"

struct Command;

class QuitCmd : public ACommand
{
public:
	QuitCmd(){}
	~QuitCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};