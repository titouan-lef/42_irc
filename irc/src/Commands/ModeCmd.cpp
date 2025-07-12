#include <cstdlib>
#include "ModeCmd.hpp"
#include "UserManager.hpp"
#include <ChanServ.hpp>
#include "NeedMoreParamsException.hpp"
#include "NoSuchChannelException.hpp"
#include "NoSuchNickException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InvalidModeParamException.hpp"
#include "UserNotInChannelException.hpp"

void ModeCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 1)
		throw NeedMoreParamsException(cmd.name);
	std::string channelName = cmd.arg.at(0);
	Channel& channel = ChanServ::getInstance().findChannel(channelName);
	if (!channel.isConnected(socket))
		throw UserNotInChannelException(UserManager::getInstance().getUserNickname(socket), channel.getName());
	if (!channel.isOperator(socket))
		throw ChannelPrivilegesNeeded(channel.getName());
	if (cmd.arg.size() == 1)
	{
		channel.sendActualModes(socket);
		return;
	}
	std::vector<Option> options = parseCommand(cmd, socket);
	options = executeModeOptions(socket, channel, options);
	std::string msg = builtBroadcastMsg(options);
	if (!msg.empty())
		channel.sendChangedModes(socket, msg);
}

std::string ModeCmd::builtBroadcastMsg(const std::vector<ModeCmd::Option>& appliedOptions) const
{
	std::string options;
	std::string args;
	std::vector<Option>::const_iterator it = appliedOptions.begin();

	if (it == appliedOptions.end())
		return "";
	int lastIsAddFlag = -1;
	for (; it < appliedOptions.end(); ++it)
	{
		if (lastIsAddFlag != it->isAddFlag && it->isAddFlag)
			options += "+";
		else if (lastIsAddFlag != it->isAddFlag)
			options += "-";
		lastIsAddFlag = it->isAddFlag;
		options += it->flag;
		if (!it->argument.empty())
			args += " " + it->argument;
	}
	return options + args;
}


std::vector<ModeCmd::Option> ModeCmd::parseCommand(const Command& cmd, const int socket) const
{
	std::vector<Option> optionList;
	argIterator it = cmd.arg.begin() + 1;
	while (it < cmd.arg.end())
	{
		addOptionInList(cmd, socket, optionList, it);
		++it;
	}
	return optionList;/** @warning max 3 arg */
}

void ModeCmd::addOptionInList(const Command &cmd, const int socket, std::vector<Option> &optionList, argIterator& it) const
{
	const std::string& flags = *it;
	int isAddFlag = -1;
	for (std::size_t i = 0; i < flags.length(); ++i)
	{
		if (flags[i] == '+' || flags[i] == '-')
		{
			isAddFlag = (flags[i] == '+');
			continue;
		}
		if (isAddFlag == -1)
			continue; /** @todo throw UnknownModeException ou pas*/
		try
		{
			Option option = {flags[i], isAddFlag, ""};
			checkOptionAndAddArg(cmd, it, option);
			optionList.push_back(option);
		}
		catch(const IrcException& e)
		{
			e.sendError(socket);
		}
	}
}

void ModeCmd::checkOptionAndAddArg(const Command& cmd, argIterator& it, Option& option) const
{
	const std::string validFlag = "itlok";
	const std::size_t posFlagArg = 2;

	std::size_t pos = validFlag.find(option.flag);
	if (pos == std::string::npos)
		throw UnknownModeException(option.flag);
	if ((pos == posFlagArg && option.isAddFlag) || pos > posFlagArg)
		option.argument = defineOptionArg(cmd, it);
}

const std::string ModeCmd::defineOptionArg(const Command& cmd, argIterator& it) const
{
	++it;
	if (it >= cmd.arg.end())
		throw NeedMoreParamsException(cmd.name);
	return *it;
}

std::vector<ModeCmd::Option> ModeCmd::executeModeOptions(const int socket, Channel& channel, const std::vector<ModeCmd::Option>& options) const
{
	std::vector<ModeCmd::Option> result;
	for (std::vector<Option>::const_iterator it = options.begin() ; it < options.end(); ++it)
	{
		try
		{
			bool isOperationSuccessfull = executeModeOption(channel, *it);
			if (isOperationSuccessfull)
				result.push_back(*it);
		}
		catch(const IrcException& e)
		{
			e.sendError(socket);
		}
	}
	return result;
}

bool ModeCmd::executeModeOption(Channel& channel, const ModeCmd::Option& option) const
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

bool ModeCmd::inviteMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
		return channel.enableInviteRestriction();
	return channel.disableInviteRestriction();
}

bool ModeCmd::topicMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
		return channel.enableTopicRestriction();
	return channel.disableTopicRestriction();
}

bool ModeCmd::passwordMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
		return channel.setPassword(option.argument);
	return channel.disablePassword(option.argument);
}

bool ModeCmd::operatorMode(Channel& channel, const ModeCmd::Option& option) const
{
	int targetSocket = UserManager::getInstance().findUserSocket(option.argument);
	if (!channel.isConnected(targetSocket))
	{
		std::string nickname = UserManager::getInstance().getUserNickname(targetSocket);
		throw UserNotInChannelException(nickname, channel.getName());
	}
	if (option.isAddFlag)
		return channel.addNewOperator(targetSocket);
	return channel.removeOperator(targetSocket);
}

bool ModeCmd::limitMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
	{
		int limit = std::atoi(option.argument.c_str());
		if (limit == 0)
		{
			std::vector<std::string> args;
			args.push_back(channel.getName());
			args.push_back(std::string(1, option.flag));
			args.push_back(option.argument);
			throw InvalidModeParamException(args);
		}
		return channel.setLimitNbUsers(limit);
	}
	return channel.disableLimitNbUsers();
}