#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class NoSuchChannelException: public IrcException
{
public:
	virtual ~NoSuchChannelException() throw() {}

	NoSuchChannelException(const int socket, const std::string& channelName);
};
