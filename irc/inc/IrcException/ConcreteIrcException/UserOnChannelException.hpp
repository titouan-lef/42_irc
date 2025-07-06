#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class UserOnChannelException: public IrcException
{
public:
	virtual ~UserOnChannelException() throw() {}

	UserOnChannelException(const int socket, const std::vector<std::string>& arg);
};
