#pragma once

#include "ACommand.hpp"

#include "command.hpp"

class NickCmd : public ACommand
{
public:
	NickCmd(){}
	~NickCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;
	void parsingNickname(const std::string& Nickname) const;
};