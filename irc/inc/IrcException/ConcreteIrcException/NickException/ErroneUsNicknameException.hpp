#pragma once

#include "IrcException.hpp"
#include <iostream>

class ErroneUsNicknameException: public IrcException
{
public:
	virtual ~ErroneUsNicknameException() throw() {}

	ErroneUsNicknameException(const int socket, const std::vector<std::string>& arg, const std::string& nickname);
};