#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

	void execute(const Command& cmd, const int socket) const;
};