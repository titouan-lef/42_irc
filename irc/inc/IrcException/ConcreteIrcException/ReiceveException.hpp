#pragma once

#include "IrcException.hpp"

class ReiceveException: public IrcException
{
public:
	virtual ~ReiceveException() throw() {}

	ReiceveException();
};