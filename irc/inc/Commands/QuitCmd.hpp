#pragma once

#include "ACommand.hpp"

class QuitCmd : public ACommand
{
public:
	QuitCmd(){}
	~QuitCmd(){}

    void execute(MsgServ::Command cmd, User & user) const;
};