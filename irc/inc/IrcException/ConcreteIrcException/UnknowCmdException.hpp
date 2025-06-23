#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknowCmdException: public IrcException
{
public:
	virtual ~UnknowCmdException() throw() {}

	UnknowCmdException(const std::string& nickname);
};