#pragma once

#include "ACommand.hpp"

class PongCmd : public ACommand
{
public:
	PongCmd(){}
	~PongCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};