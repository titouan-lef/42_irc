#pragma once

#include "ACommand.hpp"

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};