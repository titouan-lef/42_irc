#pragma once

#include "ACommand.hpp"

#include <iostream>

struct Command;

class NickCmd : public ACommand
{
public:
	NickCmd(){}
	~NickCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
	void parsingNickname(const std::string& Nickname) const;
};