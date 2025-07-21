#pragma once

#include "ACommand.hpp"

struct Command;

class KickCmd : public ACommand
{
public:
	KickCmd(){}
	~KickCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

};