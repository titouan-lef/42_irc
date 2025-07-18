#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknownCmdException: public IrcException
{
public:
	virtual ~UnknownCmdException() throw() {}

	UnknownCmdException(const std::string& commandName);
};