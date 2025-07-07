#pragma once

#include "IrcException.hpp"
#include <iostream>
#include "User.hpp"

class PasswordMismatchException: public IrcException
{
public:
	virtual ~PasswordMismatchException() throw() {}

	PasswordMismatchException();
};