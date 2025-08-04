#pragma once

#include "ACommand.hpp"

struct Command;
enum Status;

class PingCmd : public ACommand
{
public:
	PingCmd();
	~PingCmd(){}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;
};