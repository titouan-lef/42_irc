#pragma once

#include "IrcException.hpp"
#include <iostream>

class InviteOnlyException: public IrcException
{
public:
	virtual ~InviteOnlyException() throw() {}

	InviteOnlyException(int socket, const std::string& channelName, const std::string& nickname);
};