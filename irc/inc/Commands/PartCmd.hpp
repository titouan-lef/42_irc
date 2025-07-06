#pragma once

#include "ACommand.hpp"

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}

	void execute(const MsgServ::Command& cmd, const int socket) const;

private:

	std::vector<std::string> split(const std::string& str, char delimiter) const;

};