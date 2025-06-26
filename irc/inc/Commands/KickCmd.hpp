
#pragma once

#include "ACommand.hpp"

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

	void execute(MsgServ::Command cmd, User& user) const;
};