#pragma once

#include "ACommand.hpp"

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

    void execute(std::vector<std::string>args, std::string trailing, User & user) const;
};