#pragma once

#include "IrcException.hpp"

class NotRegisteredException: public IrcException
{
public:
	virtual ~NotRegisteredException() throw() {}

	NotRegisteredException();
};