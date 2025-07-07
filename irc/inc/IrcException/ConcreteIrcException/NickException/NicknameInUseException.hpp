#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class NicknameInUseException: public IrcException
{
public:
	virtual ~NicknameInUseException() throw () {}

	NicknameInUseException(const std::string& nickname);
};