#include "ModeCmd.hpp"

#include <cstdlib>

#include "command.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InvalidModeParamException.hpp"
#include "UserNotInChannelException.hpp"

void ModeCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::size_t NB_ARG_ACTUAL_MODE = 1;

	Channel& channel = checkCmd(cmd, senderSocket);
	if (cmd.arg.size() == NB_ARG_ACTUAL_MODE)
		channel.sendActualModes(senderSocket);
	else
		changeMode(cmd, senderSocket, channel);
}

Channel& ModeCmd::checkCmd(const Command& cmd, const int senderSocket) const
{
	const std::size_t NB_ARG_MIN = 1;

	if (cmd.arg.size() < NB_ARG_MIN)
		throw NeedMoreParamsException(cmd.name);

	const std::string& channelName = cmd.arg.at(0);
	Channel& channel = ChanServ::getInstance().findChannel(channelName);

	if (!channel.isConnected(senderSocket))
	{
		const std::string& nickname = UserManager::getInstance().getUserNickname(senderSocket);
		throw UserNotInChannelException(nickname, channelName);
	}
	if (!channel.isOperator(senderSocket))
		throw ChannelPrivilegesNeeded(channelName);

	return channel;
}

void ModeCmd::changeMode(const Command& cmd, const int senderSocket, Channel& channel) const
{
	std::vector<Option> optionList = parseCommand(cmd, senderSocket);
	applyAllValidOption(senderSocket, channel, optionList);
	const std::string msg = builtBroadcastMsg(optionList);
	if (!msg.empty())
		channel.sendChangedModes(senderSocket, msg);
}

std::vector<ModeCmd::Option> ModeCmd::parseCommand(const Command& cmd, const int senderSocket) const
{
	std::vector<Option> optionList;
	argIterator it = cmd.arg.begin() + 1;
	while (it < cmd.arg.end())
	{
		addOptionInList(cmd, senderSocket, optionList, it);
		++it;
	}
	return optionList;/** @warning max 3 arg */
}

void ModeCmd::addOptionInList(const Command& cmd, const int senderSocket, std::vector<Option>& optionList, argIterator& it) const
{
	const std::string& flags = *it;
	bool isAddFlag = true;
	for (std::size_t i = 0; i < flags.length(); ++i)
	{
		const char flag = flags.at(i);
		if (flag == '+' || flag == '-')
		{
			isAddFlag = (flag == '+');
			continue;
		}
		Option option = {flag, isAddFlag, ""};
		if (fillOption(cmd, senderSocket, it, option))
			optionList.push_back(option);
	}
}

bool ModeCmd::fillOption(const Command& cmd, const int senderSocket, argIterator& it, Option& option) const
{
	try
	{
		checkFlagAndItsArg(cmd, it, option);
	}
	catch(const IrcException& e)
	{
		e.sendError(senderSocket);
		return false;
	}
	return true;
}

void ModeCmd::checkFlagAndItsArg(const Command& cmd, argIterator& it, Option& option) const
{
	const std::size_t flagId = checkFlag(option.flag);
	if (flagNeedArg(option, flagId))
		option.argument = defineOptionArg(cmd, it);
}

std::size_t ModeCmd::checkFlag(const char flag) const
{
	const std::string VALID_FLAG = "itlok";

	const std::size_t flagId = VALID_FLAG.find(flag);
	if (flagId == std::string::npos)
		throw UnknownModeException(flag);
	return flagId;
}

bool ModeCmd::flagNeedArg(const Option& option, const std::size_t flagId) const
{
	const std::size_t POS_FLAG_ARG = 2;

	return (flagId > POS_FLAG_ARG || (flagId == POS_FLAG_ARG && option.isAddFlag));
}

const std::string ModeCmd::defineOptionArg(const Command& cmd, argIterator& it) const
{
	++it;
	if (it >= cmd.arg.end())
		throw NeedMoreParamsException(cmd.name);
	return *it;
}

void ModeCmd::applyAllValidOption(const int senderSocket, Channel& channel, std::vector<Option>& optionList) const
{
	for (std::vector<Option>::iterator it = optionList.begin(); it < optionList.end();)// Be carefull, no ++it.
	{
		if (applyValidOption(senderSocket, channel, *it))
			++it;
		else
			optionList.erase(it);// No ++it because of erase.
	}
}

bool ModeCmd::applyValidOption(const int senderSocket, Channel& channel, const Option& option) const
{
	try
	{
		return executeModeOption(channel, option);
	}
	catch(const IrcException& e)
	{
		e.sendError(senderSocket);
		return false;
	}
}

bool ModeCmd::executeModeOption(Channel& channel, const Option& option) const
{
	switch (option.flag)
	{
		case 'i':
			return inviteMode(channel, option);
		case 't':
			return topicMode(channel, option);
		case 'k':
			return passwordMode(channel, option);
		case 'o':
			return operatorMode(channel, option);
		case 'l':
			return limitMode(channel, option);
	}
	return false;
}

bool ModeCmd::inviteMode(Channel& channel, const Option& option) const
{
	if (option.isAddFlag)
		return channel.enableInviteRestriction();
	return channel.disableInviteRestriction();
}

bool ModeCmd::topicMode(Channel& channel, const Option& option) const
{
	if (option.isAddFlag)
		return channel.enableTopicRestriction();
	return channel.disableTopicRestriction();
}

bool ModeCmd::passwordMode(Channel& channel, const Option& option) const
{
	if (option.isAddFlag)
		return channel.setPassword(option.argument);
	return channel.disablePassword(option.argument);
}

bool ModeCmd::operatorMode(Channel& channel, const Option& option) const
{
	int targetSocket = UserManager::getInstance().findUserSocket(option.argument);
	if (!channel.isConnected(targetSocket))
	{
		const std::string& nickname = UserManager::getInstance().getUserNickname(targetSocket);
		throw UserNotInChannelException(nickname, channel.getName());
	}
	if (option.isAddFlag)
		return channel.addNewOperator(targetSocket);
	return channel.removeOperator(targetSocket);
}

bool ModeCmd::limitMode(Channel& channel, const Option& option) const
{
	if (option.isAddFlag)
		return setChannelLimit(channel, option);
	return channel.disableLimitNbUsers();
}

bool ModeCmd::setChannelLimit(Channel &channel, const Option &option) const
{
	int limit = getChannelLimit(option.argument);
	if (limit <= 0)
	{
		std::vector<std::string> args;
		args.push_back(channel.getName());
		args.push_back(std::string(1, option.flag));
		args.push_back(option.argument);
		throw InvalidModeParamException(args);
	}
	return channel.setLimitNbUsers(limit);
}

int ModeCmd::getChannelLimit(const std::string &limitArg) const
{
	const std::string AUTHORIZED_CHAR = "0123456789";

	if (limitArg.find_first_not_of(AUTHORIZED_CHAR) != std::string::npos)
		return 0;
	return std::atoi(limitArg.c_str());
}

std::string ModeCmd::builtBroadcastMsg(const std::vector<Option>& optionList) const
{
	if (optionList.empty())
		return "";
	return builtOptionMsg(optionList);
}

std::string ModeCmd::builtOptionMsg(const std::vector<Option>& optionList) const
{
	std::string allOption, allOptionArg;
	bool isAddFlag = (!optionList.at(0).isAddFlag);
	for (std::size_t i = 0; i < optionList.size(); ++i)
	{
		updateFlagMsg(optionList.at(i), allOption, isAddFlag);
		updateArgMsg(optionList.at(i), allOptionArg);
	}
	return allOption + allOptionArg;
}

void ModeCmd::updateFlagMsg(const Option &option, std::string& allOption, bool &isAddFlag) const
{
	if (isAddFlag != option.isAddFlag)
	{
		allOption += (option.isAddFlag ? "+" : "-");
		isAddFlag = option.isAddFlag;
	}
	allOption += option.flag;
}

void ModeCmd::updateArgMsg(const Option &option, std::string &allOptionArg) const
{
	if (!option.argument.empty())
		allOptionArg += " " + option.argument;
}
