#pragma once

#include "ACommand.hpp"

class NickCmd : public ACommand
{
public:
	NickCmd(){}
	~NickCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};