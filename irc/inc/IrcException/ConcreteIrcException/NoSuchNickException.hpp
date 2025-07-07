#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class NoSuchNickException: public IrcException
{
public:
	virtual ~NoSuchNickException() throw() {}

	NoSuchNickException(const std::string& nickname);
};
