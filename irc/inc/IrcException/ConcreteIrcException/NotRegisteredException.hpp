#pragma once

#include "IrcException.hpp"
#include <iostream>
#include "User.hpp"

class NotRegisteredException: public IrcException
{
public:
	virtual ~NotRegisteredException() throw() {}

	NotRegisteredException(int socket, const std::string& nickname = User::getDefaultNickname());
};