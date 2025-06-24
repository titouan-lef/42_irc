#pragma once

#include "IrcException.hpp"
#include <iostream>

class NotRegisteredException: public IrcException
{
public:
	virtual ~NotRegisteredException() throw() {}

	NotRegisteredException();

};