#pragma once

#include "IrcException.hpp"
#include <iostream>

class ClientDisconnectedException: public IrcException
{
public:
	virtual ~ClientDisconnectedException() throw() {}

	ClientDisconnectedException();
};