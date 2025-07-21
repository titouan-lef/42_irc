#pragma once

#include "IrcException.hpp"

#include <iostream>

class NotOnChannelException: public IrcException
{
public:
	virtual ~NotOnChannelException() throw() {}

	NotOnChannelException(const std::string& channelName);
};
