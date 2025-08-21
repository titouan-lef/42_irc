#pragma once

#include "IrcException.hpp"

#include <iostream>

class InvalidModeParamException: public IrcException
{
public:
	virtual ~InvalidModeParamException() throw() {}

	InvalidModeParamException(const std::string& channelName, const char optionFlag, const std::string& optionArg);
};
