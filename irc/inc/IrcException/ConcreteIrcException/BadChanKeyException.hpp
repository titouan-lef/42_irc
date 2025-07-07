#pragma once

#include "IrcException.hpp"
#include <iostream>

class BadChanKeyException: public IrcException
{
public:
	virtual ~BadChanKeyException() throw() {}

	BadChanKeyException(const std::string& channelName);
};