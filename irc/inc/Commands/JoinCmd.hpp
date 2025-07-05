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
	void tryConnection(const std::string& channel, const int socket, const std::string* password) const;
	
	std::vector<std::string> splitArgument(const std::string& arg);
	bool isValidChannelName(const std::string& channelName) const;
	bool isPasswordProtected(const std::string& channelName) const;
};