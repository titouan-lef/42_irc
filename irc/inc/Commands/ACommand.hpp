#pragma once

#include <iostream>
#include <vector>

struct Command;

class ACommand
{
public:
	ACommand(){}
	virtual ~ACommand(){}

	virtual void execute(const Command& cmd, const int senderSocket) const = 0;

protected:

	std::vector<std::string> splitTargets(const std::string& str) const;
	const std::string truncateTrailing(const std::string& trailing) const;
	std::string getLastParam(const Command& cmd, const size_t nbArgsRequiered) const;
	std::string getLastParam(const Command& cmd, const size_t argsMin, const size_t argsMax) const;
	size_t getNbArg(const Command& cmd) const;
};