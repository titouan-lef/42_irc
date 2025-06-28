#pragma once

#include "IrcException.hpp"
#include <iostream>

class PasswordMismatchException: public IrcException
{
public:
	virtual ~PasswordMismatchException() throw() {}

	PasswordMismatchException(const int socket, const std::string& nickname = "*");
};