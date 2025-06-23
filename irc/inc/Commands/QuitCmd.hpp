#pragma once

#include "ACommand.hpp"

class QuitCmd : public ACommand
{
public:
	QuitCmd(){}
	~QuitCmd(){}

	void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};