#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class UserOnChannelException: public IrcException
{
public:
	virtual ~UserOnChannelException() throw() {}

	UserOnChannelException(const std::vector<std::string>& arg);
};
