#pragma once

#include "IrcException.hpp"

class MessageTooLongException: public IrcException
{
public:
	virtual ~MessageTooLongException() throw() {}

	MessageTooLongException();
};