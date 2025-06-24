#pragma once

#include "IrcException.hpp"
#include <iostream>

class AlreadyRegistredException: public IrcException
{
public:
	virtual ~AlreadyRegistredException() {}

	AlreadyRegistredException(const std::string& name);
};