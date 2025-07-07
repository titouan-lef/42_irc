#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class ListCmd : public ACommand
{
public:
	ListCmd(){}
	~ListCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

};