#pragma once

#include "IrcException.hpp"
#include <iostream>

class NeedMoreParamsException: public IrcException
{
public:
	virtual ~NeedMoreParamsException() throw() {}

	NeedMoreParamsException(const std::string& nickname);
};