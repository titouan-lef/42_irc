#pragma once

#include "ACommand.hpp"

class PartCmd : public ACommand
{
public:
	PartCmd() {}
	~PartCmd() {}
	
	void execute(MsgServ::Command cmd, User& user) const;
};