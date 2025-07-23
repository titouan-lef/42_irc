#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class JoinCmd : public ACommand
{
public:
	JoinCmd(){}
	~JoinCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	bool isPartAllCmd(const std::size_t nbArg, const std::string& lastParam) const;
	void tryConnection(const std::string& channel, const int socket, const std::string* password) const;
	bool isPasswordProtected(const std::string& channelName) const;
};