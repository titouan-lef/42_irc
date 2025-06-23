#pragma once

#include <iostream>
#include <vector>
#include "User.hpp"

class ACommand
{
public:
	ACommand(){}
	virtual ~ACommand(){}

    virtual void execute(std::vector<std::string>args, std::string trailing, User & user) const = 0;
};