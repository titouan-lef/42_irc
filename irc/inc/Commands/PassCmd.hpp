#pragma once

#include "ACommand.hpp"

class PassCmd : public ACommand
{
public:
	PassCmd() {}
	~PassCmd() {}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};