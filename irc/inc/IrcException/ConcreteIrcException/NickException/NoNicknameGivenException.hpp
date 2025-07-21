#pragma once

#include "IrcException.hpp"

class NoNicknameGivenException: public IrcException
{
public:
	virtual ~NoNicknameGivenException() throw () {}

	NoNicknameGivenException();
};