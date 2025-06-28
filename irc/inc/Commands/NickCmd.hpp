#pragma once

#include "ACommand.hpp"

class NickCmd : public ACommand
{
public:
	NickCmd(){}
	~NickCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};