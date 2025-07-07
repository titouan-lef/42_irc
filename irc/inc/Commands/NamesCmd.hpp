#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class NamesCmd : public ACommand
{
public:
	NamesCmd(){}
	~NamesCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

};