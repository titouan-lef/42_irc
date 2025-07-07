#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class NamesCmd : public ACommand
{
public:
	NamesCmd(){}
	~NamesCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:

	std::vector<std::string> split(const std::string& str, char delimiter) const;

};