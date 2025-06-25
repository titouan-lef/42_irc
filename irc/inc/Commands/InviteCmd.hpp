#pragma once

#include "ACommand.hpp"

class InviteCmd : public ACommand
{
public:
	InviteCmd(){}
	~InviteCmd(){}

    void execute(MsgServ::Command cmd, User & user) const;
};