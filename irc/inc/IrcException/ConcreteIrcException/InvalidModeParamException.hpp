#pragma once

#include "IrcException.hpp"
#include <iostream>

class InvalidModeParamException: public IrcException
{
public:
	virtual ~InvalidModeParamException() throw() {}

	InvalidModeParamException(const int socket, const std::string& msg, const std::string& nickname);
};
