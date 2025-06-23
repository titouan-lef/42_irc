
#pragma once

#include "ACommand.hpp"

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

    void execute(std::vector<std::string>args, std::string trailing, User & user) const;
};