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
};