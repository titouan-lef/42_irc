#pragma once

#include "IrcException.hpp"

#include <iostream>
#include <vector>

class InvalidModeParamException: public IrcException
{
public:
	virtual ~InvalidModeParamException() throw() {}

	InvalidModeParamException(const std::vector<std::string>& arg);
};
