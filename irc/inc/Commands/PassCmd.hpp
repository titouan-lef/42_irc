#pragma once

#include "ACommand.hpp"

class PassCmd : public ACommand
{
public:
	PassCmd() {}
	~PassCmd() {}

	void execute(MsgServ::Command cmd, User& user) const;
};