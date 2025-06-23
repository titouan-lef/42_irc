#pragma once

#include "ACommand.hpp"

class PingCmd : public ACommand
{
public:
	PingCmd(){}
	~PingCmd(){}

	void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};