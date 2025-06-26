#pragma once

#include "ACommand.hpp"

class PingCmd : public ACommand
{
public:
	PingCmd(){}
	~PingCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};