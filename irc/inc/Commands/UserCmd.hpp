#pragma once

#include "ACommand.hpp"

class UserCmd : public ACommand
{
public:
	UserCmd(){}
	~UserCmd(){}

    void execute(MsgServ::Command cmd, User & user) const;
};