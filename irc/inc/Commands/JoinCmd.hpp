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
	void tryConnection(const std::string& channel, const int socket) const;
	void tryConnection(const std::string& channel, std::vector<std::string>::iterator& itPwd, const int socket) const;
	void tryConnection(const std::string& channel, std::string invalidPassword, const int socket) const;

	std::vector<std::string> splitArgument(const std::string& arg);
	bool checkChannelName(const std::string& channelName);
};