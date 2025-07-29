#pragma once

#include <iostream>
#include <vector>

struct Command;

class ACommand
{
public:
	virtual ~ACommand(){}

	virtual void execute(const Command& cmd, const int senderSocket) const = 0;

protected:
	ACommand(const std::size_t nbArgMin, const std::size_t nbArgMax);

	void checkArgSize(const Command& cmd) const;
	std::vector<std::string> splitTargets(const std::string& str) const;

	const std::size_t nbArgMin_;
	const std::size_t nbArgMax_;
};