#pragma once

#include "IrcException.hpp"
#include <iostream>

class UnknowCmdException: public IrcException
{
public:
	virtual ~UnknowCmdException() throw() {}

	UnknowCmdException(int socket, const std::string& commandName, const std::string& nickname);
};