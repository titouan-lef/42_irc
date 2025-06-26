#pragma once

#include "IrcException.hpp"
#include <iostream>

class AlreadyRegistredException: public IrcException
{
public:
	virtual ~AlreadyRegistredException() throw() {}

	AlreadyRegistredException(int socket, const std::string& nickname);
};