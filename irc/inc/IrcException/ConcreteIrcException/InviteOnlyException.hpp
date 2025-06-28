#pragma once

#include "IrcException.hpp"
#include <iostream>

class InviteOnlyException: public IrcException
{
public:
	virtual ~InviteOnlyException() throw() {}

	InviteOnlyException(const int socket, const std::string& channelName);
};