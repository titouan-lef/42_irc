#pragma once

#include "IrcException.hpp"
#include <iostream>

class TooManyTargetsException: public IrcException
{
public:
	virtual ~TooManyTargetsException() throw() {}

	TooManyTargetsException(const std::string& target);
};