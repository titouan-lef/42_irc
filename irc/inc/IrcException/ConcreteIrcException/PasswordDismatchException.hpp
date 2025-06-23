#pragma once

#include "IrcException.hpp"
#include <iostream>

class PasswordDismatchException: public IrcException
{
public:
	virtual ~PasswordDismatchException() {}

	PasswordDismatchException();
};