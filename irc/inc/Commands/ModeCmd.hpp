#pragma once

#include "ACommand.hpp"

#include <iostream>
#include <vector>

class Channel;
struct Command;

class ModeCmd : public ACommand
{
public:
	ModeCmd(){}
	~ModeCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	typedef std::vector<std::string>::const_iterator argIterator;
	struct Option {
		char flag;
		bool isAddFlag;
		std::string argument;
	};

	std::vector<Option> parseCommand(const Command& cmd, const int socket) const;
	void addOptionInList(const Command& cmd, const int socket, std::vector<Option>& optionList, argIterator& it) const;
	void checkOptionAndAddArg(const Command& cmd, argIterator& it, Option& option) const;
	const std::string defineOptionArg(const Command& cmd, argIterator& it) const;
	std::vector<ModeCmd::Option> executeModeOptions(const int socket, Channel& channel, const std::vector<Option>& options) const;
	bool executeModeOption(Channel& channel, const Option& option) const;
	std::string builtBroadcastMsg(const std::vector<ModeCmd::Option>& appliedOptions) const;

	bool inviteMode(Channel& channel, const Option& option) const;
	bool topicMode(Channel& channel, const Option& option) const;
	bool passwordMode(Channel& channel, const Option& option) const;
	bool operatorMode(Channel& channel, const Option& option) const;
	bool limitMode(Channel& channel, const Option& option) const;
};
