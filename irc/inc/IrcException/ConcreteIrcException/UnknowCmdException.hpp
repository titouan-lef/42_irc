#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknowCmdException: public IrcException
{
public:
	virtual ~UnknowCmdException() throw() {}

	UnknowCmdException(const int socket, const std::string& commandName);
};