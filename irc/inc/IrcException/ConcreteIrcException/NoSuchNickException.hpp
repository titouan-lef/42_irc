#pragma once

#include "IrcException.hpp"
#include <iostream>

class NoSuchNickException: public IrcException
{
public:
	virtual ~NoSuchNickException() {}

	NoSuchNickException(const std::string& nickname);
};
