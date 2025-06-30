#pragma once

#include "IrcException.hpp"
#include <iostream>
#include <vector>

class NicknameInUseException: public IrcException
{
public:
	virtual ~NicknameInUseException() throw () {}

	NicknameInUseException(const int socket, const std::vector<std::string>& arg, const std::string& nickname);
};