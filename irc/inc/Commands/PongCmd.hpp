#pragma once

#include "ACommand.hpp"

class PongCmd : public ACommand
{
public:
	PongCmd(){}
	~PongCmd(){}
	
	void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};