#pragma once

#include "ACommand.hpp"

struct Command;

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
};