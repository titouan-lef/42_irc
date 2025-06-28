#pragma once

#include "ACommand.hpp"

class QuitCmd : public ACommand
{
public:
	QuitCmd(){}
	~QuitCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};