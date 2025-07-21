#pragma once

#include "IrcException.hpp"

#include <iostream>

class UserNotInChannelException: public IrcException
{
public:
	virtual ~UserNotInChannelException() throw() {}

	UserNotInChannelException(const std::string& nickname, const std::string& channelName);
};
