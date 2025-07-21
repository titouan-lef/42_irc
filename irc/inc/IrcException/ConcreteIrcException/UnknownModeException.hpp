#pragma once

#include "IrcException.hpp"

class UnknownModeException: public IrcException
{
public:
	virtual ~UnknownModeException() throw() {}

	UnknownModeException(const char& mode);

};
