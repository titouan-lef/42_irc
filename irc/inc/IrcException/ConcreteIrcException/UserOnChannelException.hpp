#pragma once

#include "IrcException.hpp"

#include <iostream>

class UserOnChannelException: public IrcException
{
public:
	virtual ~UserOnChannelException() throw() {}

	UserOnChannelException(const std::string& username, const std::string& channelName);
};
