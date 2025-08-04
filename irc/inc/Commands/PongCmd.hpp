#pragma once

#include "ACommand.hpp"

struct Command;
enum Status;

class PongCmd : public ACommand
{
public:
	PongCmd();
	~PongCmd(){}

	void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const;
};