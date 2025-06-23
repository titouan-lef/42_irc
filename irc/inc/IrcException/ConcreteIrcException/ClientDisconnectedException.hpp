#pragma once

#include "IrcException.hpp"
#include <iostream>

class ClientDisconnectedException: public IrcException
{
public:
	virtual ~ClientDisconnectedException() {}

	ClientDisconnectedException(const std::string& nickname);
};