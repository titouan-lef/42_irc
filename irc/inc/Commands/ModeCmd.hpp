#pragma once

#include "ACommand.hpp"
#include "ChanServ.hpp"
#include <vector>

class ModeCmd : public ACommand
{
public:
	ModeCmd(){}
	~ModeCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;

private:
	struct Option {
		char type;
		int setOrRemove; // 1 -> Set 0 -> Remove
		std::string argument;
	};

	std::vector<Option> parseCommand(const MsgServ::Command& cmd, const int socket) const;
	void processModeChar(char modeChar, int& setOrRemove, size_t& argIndex, const MsgServ::Command& cmd, int socket, std::vector<ModeCmd::Option>& options) const;

};
