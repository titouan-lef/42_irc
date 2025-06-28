#pragma once

#include "ACommand.hpp"

class InviteCmd : public ACommand
{
public:
	InviteCmd(){}
	~InviteCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};