#pragma once

#include "ACommand.hpp"

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};