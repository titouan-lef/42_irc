#pragma once

#include <iostream>
#include <vector>
#include "User.hpp"
#include "MsgServ.hpp"

class ACommand
{
public:
	ACommand(){}
	virtual ~ACommand(){}

	virtual void execute(const MsgServ::Command& cmd, const int socket) const = 0;
};