#pragma once

#include "IrcException.hpp"
#include <iostream>

class SendException: public IrcException
{
public:
	virtual ~SendException() throw() {}

	SendException(const int socket, const std::vector<std::string>& arg, const std::string& nickname);
};