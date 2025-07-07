#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;

private:

	std::vector<std::string> split(const std::string& str, char delimiter) const;

};