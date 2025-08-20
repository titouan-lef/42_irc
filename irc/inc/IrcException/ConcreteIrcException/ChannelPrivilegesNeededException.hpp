#pragma once

#include "IrcException.hpp"

#include <iostream>

class ChannelPrivilegesNeededException: public IrcException
{
public:
	virtual ~ChannelPrivilegesNeededException() throw() {}

	ChannelPrivilegesNeededException(const std::string& channelName);

};