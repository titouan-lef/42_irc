#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class ListCmd : public ACommand
{
public:
	ListCmd(){}
	~ListCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmd(const Command& cmd) const;
	void sendInfoAllChannel(const int senderSocket) const;
	void sendInfoSpecificChannel(const Command& cmd, const int senderSocket) const;
	void sendInfoOneChannel(const int senderSocket, const std::string& channelName) const;
};