#pragma once

#include "ACommand.hpp"

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
private:
	std::vector<std::string> splitArgument(const std::string& arg) const;
	bool checkChannelName(const std::string& channelName) const;
};