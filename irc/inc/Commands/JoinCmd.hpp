#pragma once

#include "ACommand.hpp"

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

    void execute(std::vector<std::string>args, std::string trailing, User & user) const;
};