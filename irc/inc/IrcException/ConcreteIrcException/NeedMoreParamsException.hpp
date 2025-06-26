#pragma once

#include "IrcException.hpp"
#include <iostream>

class NeedMoreParamsException: public IrcException
{
public:
	virtual ~NeedMoreParamsException() throw() {}

	NeedMoreParamsException(int socket, const std::string& commandName, const std::string& nickname);
};