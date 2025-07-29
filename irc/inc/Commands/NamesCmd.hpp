#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class NamesCmd : public ACommand
{
public:
	NamesCmd();
	~NamesCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void sendListUserInEachChannel(const int senderSocket) const;
	void sendListUserInSpecificChannel(const Command& cmd, const int senderSocket) const;
	void sendListUserInOneChannel(const int senderSocket, const std::string& channelName) const;
};