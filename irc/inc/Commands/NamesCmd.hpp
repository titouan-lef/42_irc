#pragma once

#include "ACommand.hpp"

struct Command;

class NamesCmd : public ACommand
{
public:
	NamesCmd(){}
	~NamesCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

};