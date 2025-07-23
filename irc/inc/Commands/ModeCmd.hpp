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

	Channel& checkCmd(const Command& cmd, const int senderSocket) const;
	void changeMode(const Command& cmd, const int senderSocket, Channel& channel) const;

	std::vector<Option> parseCommand(const Command& cmd, const int senderSocket) const;
	void addOptionInList(const Command& cmd, const int senderSocket, std::vector<Option>& optionList, argIterator& it) const;
	bool fillOption(const Command &cmd, const int senderSocket, argIterator& it, Option &option) const;
	void checkFlagAndItsArg(const Command& cmd, argIterator& it, Option& option) const;
	std::size_t checkFlag(const char flag) const;
	bool flagNeedArg(const Option& option, const std::size_t flagId) const;
	const std::string defineOptionArg(const Command& cmd, argIterator& it) const;

	void applyAllValidOption(const int senderSocket, Channel& channel, std::vector<Option>& optionList) const;
	bool applyValidOption(const int senderSocket, Channel& channel, const Option& option) const;
	bool executeModeOption(Channel& channel, const Option& option) const;

	bool inviteMode(Channel& channel, const Option& option) const;
	bool topicMode(Channel& channel, const Option& option) const;
	bool passwordMode(Channel& channel, const Option& option) const;
	bool operatorMode(Channel& channel, const Option& option) const;
	bool limitMode(Channel& channel, const Option& option) const;
	bool setChannelLimit(Channel& channel, const Option& option) const;

	std::string builtBroadcastMsg(const std::vector<Option>& optionList) const;
	std::string builtOptionMsg(const std::vector<Option>& optionList) const;
	void updateFlagMsg(const Option& option, std::string& allOption, bool& isAddFlag) const;
	void updateArgMsg(const Option& option, std::string& allOptionArg) const;
};
