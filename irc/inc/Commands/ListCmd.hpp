#pragma once

#include "ACommand.hpp"

struct Command;

class ListCmd : public ACommand
{
public:
	ListCmd(){}
	~ListCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

};