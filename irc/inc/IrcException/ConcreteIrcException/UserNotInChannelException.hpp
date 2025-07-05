#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class UserNotInChannelException: public IrcException
{
public:
	virtual ~UserNotInChannelException() throw() {}

	UserNotInChannelException(const int socket, const std::vector<std::string>& arg, const std::string& nickname);
};
