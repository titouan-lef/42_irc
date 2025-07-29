#pragma once

#include "ACommand.hpp"

class User;
struct Command;

class PassCmd : public ACommand
{
public:
	PassCmd();
	~PassCmd() {}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	void checkCmd(const Command& cmd, const User& user) const;
};