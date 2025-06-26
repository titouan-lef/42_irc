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

	virtual void execute(MsgServ::Command cmd, User& user) const = 0;
};