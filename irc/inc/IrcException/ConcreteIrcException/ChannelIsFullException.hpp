#pragma once

#include "IrcException.hpp"
#include <iostream>

class ChannelIsFullException: public IrcException
{
public:
	virtual ~ChannelIsFullException() throw() {}

	ChannelIsFullException(const std::string& name);
};