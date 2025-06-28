#pragma once

#include "ACommand.hpp"

class UserCmd : public ACommand
{
public:
	UserCmd(){}
	~UserCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
	bool validUsername(std::string username) const;
};