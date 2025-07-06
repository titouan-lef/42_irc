#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class NotOnChannelException: public IrcException
{
public:
	virtual ~NotOnChannelException() throw() {}

	NotOnChannelException(const int socket, const std::string& channelName);
};
