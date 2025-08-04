#pragma once

#include "ACommand.hpp"

struct Command;
enum Status;

class PassCmd : public ACommand
{
public:
	PassCmd();
	~PassCmd() {}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;

private:
	void checkCmd(const Command& cmd) const;
};