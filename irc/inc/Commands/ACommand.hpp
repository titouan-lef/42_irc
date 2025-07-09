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
	std::string getLastParam(const Command& cmd, const size_t nbArgsRequiered) const;
	std::string getLastParam(const Command& cmd, const size_t argsMin, const size_t argsMax) const;
	size_t getNbArgs(const Command& cmd) const;
};