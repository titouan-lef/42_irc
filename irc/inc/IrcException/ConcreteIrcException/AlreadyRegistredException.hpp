#pragma once

#include "IrcException.hpp"

class AlreadyRegistredException: public IrcException
{
public:
	virtual ~AlreadyRegistredException() throw() {}

	AlreadyRegistredException();
};