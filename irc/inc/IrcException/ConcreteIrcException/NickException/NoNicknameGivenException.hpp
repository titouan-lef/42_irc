#pragma once

#include "IrcException.hpp"
#include <iostream>

class NoNicknameGivenException: public IrcException
{
public:
	virtual ~NoNicknameGivenException() throw () {}

	NoNicknameGivenException(const int socket, const std::string& nickname);
};