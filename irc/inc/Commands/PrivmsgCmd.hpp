#pragma once

#include "ACommand.hpp"

class PrivmsgCmd : public ACommand
{
public:
	PrivmsgCmd(){}
	~PrivmsgCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};