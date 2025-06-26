#pragma once

#include "IrcException.hpp"
#include <iostream>

class NoSuchNickException: public IrcException
{
public:
	virtual ~NoSuchNickException() throw() {}

	NoSuchNickException(int socket, const std::vector<std::string>& arg, const std::string& nickname);
};
