#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class UserCmd : public ACommand
{
public:
	UserCmd(){}
	~UserCmd(){}

	void execute(const Command& cmd, const int socket) const;
	bool validUsername(std::string username) const;
};