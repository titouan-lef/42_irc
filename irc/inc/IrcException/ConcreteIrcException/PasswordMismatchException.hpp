#pragma once

#include "IrcException.hpp"

class PasswordMismatchException: public IrcException
{
public:
	virtual ~PasswordMismatchException() throw() {}

	PasswordMismatchException();
};