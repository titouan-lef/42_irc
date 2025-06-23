#pragma once

#include "IrcException.hpp"
#include <iostream>

class ReiceveException: public IrcException
{
public:
	virtual ~ReiceveException() {}

	ReiceveException(const std::string& nickname);
};