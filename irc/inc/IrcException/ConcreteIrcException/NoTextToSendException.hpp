#pragma once

#include "IrcException.hpp"

class NoTextToSendException: public IrcException
{
public:
	virtual ~NoTextToSendException() throw() {}

	NoTextToSendException();
};