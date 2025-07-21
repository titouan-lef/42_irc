#pragma once

#include "IrcException.hpp"

#include <iostream>

class NoSuchChannelException: public IrcException
{
public:
	virtual ~NoSuchChannelException() throw() {}

	NoSuchChannelException(const std::string& channelName);
};
