#pragma once

#include "IrcException.hpp"
#include <iostream>
#include "utils.hpp"

class NoSuchNickException: public IrcException
{
public:
	NoSuchNickException(const std::string& nickname): IrcException(CommandResponse(401, "ERR_NOSUCHNICK", nickname + " :No such nick/channel")) {}
};
