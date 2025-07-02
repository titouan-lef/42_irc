#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class UnknownModeException: public IrcException
{
public:
	virtual ~UnknownModeException() throw() {}

	UnknownModeException(const int socket, const char& mode);

};
