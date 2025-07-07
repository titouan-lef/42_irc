#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:

	void tryConnection(const std::string& channel, const int socket, const std::string* password) const;
	bool isValidChannelName(const std::string& channelName) const;
	bool isPasswordProtected(const std::string& channelName) const;

};