#pragma once

#include "IrcException.hpp"

class ClientDisconnectedException: public IrcException
{
public:
	virtual ~ClientDisconnectedException() throw() {}

	ClientDisconnectedException();
};