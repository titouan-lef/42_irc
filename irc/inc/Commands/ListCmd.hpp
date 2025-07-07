#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class ListCmd : public ACommand
{
public:
	ListCmd(){}
	~ListCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:

	std::vector<std::string> split(const std::string& str, char delimiter) const;

};