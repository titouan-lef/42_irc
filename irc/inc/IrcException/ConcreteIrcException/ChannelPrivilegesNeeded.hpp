#pragma once

#include "IrcException.hpp"

#include <iostream>

class ChannelPrivilegesNeeded: public IrcException
{
public:
	virtual ~ChannelPrivilegesNeeded() throw() {}

	ChannelPrivilegesNeeded(const std::string& channelName);

};