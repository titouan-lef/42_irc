#pragma once

#include "IrcException.hpp"

#include <iostream>

class NicknameInUseException: public IrcException
{
public:
	virtual ~NicknameInUseException() throw () {}

	NicknameInUseException(const std::string& nickname);
};