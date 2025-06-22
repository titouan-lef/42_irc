#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknowCmd: public IrcException
{
public:
	virtual ~UnknowCmd() {}

	UnknowCmd(const std::string& nickname);
};