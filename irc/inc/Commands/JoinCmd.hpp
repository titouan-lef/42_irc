#pragma once

#include "ACommand.hpp"

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};