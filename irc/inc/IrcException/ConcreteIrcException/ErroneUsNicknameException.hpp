#pragma once

#include "IrcException.hpp"
#include <iostream>

class ErroneUsNicknameException: public IrcException
{
public:
	virtual ~ErroneUsNicknameException() throw() {}

	ErroneUsNicknameException(const std::string& nickname);
};