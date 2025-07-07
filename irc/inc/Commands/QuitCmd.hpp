#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class QuitCmd : public ACommand
{
public:
	QuitCmd(){}
	~QuitCmd(){}

	void execute(const Command& cmd, const int socket) const;
};