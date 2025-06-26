#pragma once

#include "ACommand.hpp"

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd(){}
	~PrivmsgCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};