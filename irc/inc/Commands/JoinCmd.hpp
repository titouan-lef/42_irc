#pragma once

#include "ACommand.hpp"

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
};

std::vector<std::string> splitArgument(const std::string& arg);
bool checkChannelName(const std::string& channelName);