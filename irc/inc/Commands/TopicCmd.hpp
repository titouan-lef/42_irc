#pragma once

#include "ACommand.hpp"

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

    void execute(MsgServ::Command cmd, User & user) const;
};