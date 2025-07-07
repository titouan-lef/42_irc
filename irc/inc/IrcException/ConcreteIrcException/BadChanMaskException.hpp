#pragma once

#include "IrcException.hpp"
#include <iostream>

class BadChanMaskException: public IrcException
{
public:
	virtual ~BadChanMaskException() throw() {}

	BadChanMaskException(const std::string& channelName);
};