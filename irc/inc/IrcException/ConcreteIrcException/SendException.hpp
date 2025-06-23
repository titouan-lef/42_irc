#pragma once

#include "IrcException.hpp"
#include <iostream>

class SendException: public IrcException
{
public:
	virtual ~SendException() {}

	SendException(const std::string& nickname);
};