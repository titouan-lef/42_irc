#pragma once

#include "IrcException.hpp"

class BadChanKeyException: public IrcException
{
public:
	virtual ~BadChanKeyException() throw() {}

	BadChanKeyException(const std::string& channelName);
};