#pragma once

#include <iostream>

#include "command.hpp"

class ACommand
{
public:
	ACommand(){}
	virtual ~ACommand(){}

	virtual void execute(const Command& cmd, const int senderSocket) const = 0;

protected:

	std::vector<std::string> split(const std::string& str, const char delimiter) const;
	Command truncateTrailing(const Command& cmd) const;

};