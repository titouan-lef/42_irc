#pragma once

#include "IrcException.hpp"
#include <iostream>

class ChannelPrivilegesNeeded: public IrcException
{
public:
	virtual ~ChannelPrivilegesNeeded() throw() {}

	ChannelPrivilegesNeeded(int socket, const std::string& channelName, const std::string& nickname);

};