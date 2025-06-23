#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknowCmdException: public IrcException
{
public:
	virtual ~UnknowCmdException() {}

	UnknowCmdException(const std::string& nickname);
};