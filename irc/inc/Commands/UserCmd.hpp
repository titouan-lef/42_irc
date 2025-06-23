#pragma once

#include "ACommand.hpp"

class UserCmd : public ACommand
{
public:
	UserCmd(){}
	~UserCmd(){}

	void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};