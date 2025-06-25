#pragma once

#include "ACommand.hpp"

class ModeCmd : public ACommand
{
public:
	ModeCmd(){}
	~ModeCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};