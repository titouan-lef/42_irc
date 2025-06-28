#pragma once

#include "ACommand.hpp"

class PingCmd : public ACommand
{
public:
	PingCmd(){}
	~PingCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};