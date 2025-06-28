#pragma once

#include "ACommand.hpp"

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};