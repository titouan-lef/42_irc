#pragma once

#include "IrcException.hpp"
#include <iostream>

class NoTextToSendException: public IrcException
{
public:
	virtual ~NoTextToSendException() throw() {}

	NoTextToSendException();
};