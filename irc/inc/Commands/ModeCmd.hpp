
#pragma once

#include "ACommand.hpp"

class ModeCmd : public ACommand
{
public:
	ModeCmd(){}
	~ModeCmd(){}

	void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};