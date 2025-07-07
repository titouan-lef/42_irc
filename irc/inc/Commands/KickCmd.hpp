#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

	void execute(const Command& cmd, const int socket) const;

private:

	std::vector<std::string> split(const std::string& str, char delimiter) const;

};