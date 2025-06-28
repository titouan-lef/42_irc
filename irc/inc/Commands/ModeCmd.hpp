#pragma once

#include "ACommand.hpp"

class ModeCmd : public ACommand
{
public:
	ModeCmd(){}
	~ModeCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};