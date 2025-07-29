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
	void checkCmd(const Command& cmd) const;
	bool isPartAllCmd(const std::vector<std::string>& arg) const;
	void tryConnection(const std::string& channel, const int socket, const std::string* password) const;
	bool isPasswordProtected(const std::string& channelName) const;
};