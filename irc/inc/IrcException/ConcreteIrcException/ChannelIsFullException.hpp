#pragma once

#include "IrcException.hpp"
#include <iostream>

class ChannelIsFullException: public IrcException
{
public:
	virtual ~ChannelIsFullException() throw() {}

	ChannelIsFullException(int socket, const std::string& channelName, const std::string& nickname, const std::string& name);
};