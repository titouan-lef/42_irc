#include "ModeCmd.hpp"

#include <cstdlib>

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
	std::vector<Option> options = parseCommand(cmd);
	executeModeOptions(socket, channel, options);
}

std::vector<ModeCmd::Option> ModeCmd::parseCommand(const Command& cmd) const
{
	std::vector<Option> optionList;
	argIterator it = cmd.arg.begin() + 1;
	while (it < cmd.arg.end())
	{
		addOptionInList(cmd, optionList, it);
		++it;
	}
	return optionList;/** @warning max 3 arg */
}

void ModeCmd::addOptionInList(const Command &cmd, std::vector<Option> &optionList, argIterator& it) const
{
	const std::string& flag = *it;
	if ((flag[0] != '+' && flag[0] != '-') || flag[1] == '\0')
		throw UnknownModeException(flag[0]);
	const bool isAddFlag = (flag[0] == '+');
	for (std::size_t i = 1; flag[i] != '\0'; ++i)
	{
		Option option = {flag[i], isAddFlag, ""};
		checkOptionAndAddArg(cmd, it, option);
		optionList.push_back(option);
	}
}

void ModeCmd::checkOptionAndAddArg(const Command& cmd, argIterator& it, Option& option) const
{
	const std::string validFlag = "itokl";
	const std::size_t posFlagArg = 2;

	std::size_t pos = validFlag.find(option.flag);
	if (pos == std::string::npos)
		throw UnknownModeException(option.flag);
	if (pos == posFlagArg || (pos > posFlagArg && option.isAddFlag))
		option.argument = defineOptionArg(cmd, it);
}

const std::string ModeCmd::defineOptionArg(const Command& cmd, argIterator& it) const
{
	++it;
	if (it >= cmd.arg.end())
		throw NeedMoreParamsException(cmd.name);
	return *it;
}

void ModeCmd::executeModeOptions(const int socket, Channel& channel, const std::vector<ModeCmd::Option>& options) const
{
	for (std::vector<Option>::const_iterator it = options.begin() ; it < options.end(); ++it)
	{
		try
		{
			executeModeOption(channel, *it);
		}
		catch(const IrcException& e)
		{
			e.sendError(socket);
		}
	}
}

void ModeCmd::executeModeOption(Channel& channel, const ModeCmd::Option& option) const
{
	switch (option.flag)
	{
		case 'i':
			inviteMode(channel, option);
			break;
		case 't':
			topicMode(channel, option);
			break;
		case 'k':
			passwordMode(channel, option);
			break;
		case 'o':
			operatorMode(channel, option);
			break;
		case 'l':
			limitMode(channel, option);
			break;
	}
}

void ModeCmd::inviteMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
	{
		channel.enableInviteRestriction();
		return ;
	}
	channel.disableInviteRestriction();
}

void ModeCmd::topicMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
	{
		channel.enableTopicRestriction();
		return ;
	}
	channel.disableTopicRestriction();
}

void ModeCmd::passwordMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.isAddFlag)
	{
		channel.setPassword(option.argument);
		return ;
	}
	channel.disablePassword();
}

void ModeCmd::operatorMode(Channel& channel, const ModeCmd::Option& option) const
{
	int targetSocket = UserManager::getInstance().findUserSocket(option.argument);
	if (!channel.isConnected(targetSocket))
	{
		std::string nickname = UserManager::getInstance().getUserNickname(targetSocket);
		throw UserNotInChannelException(nickname, channel.getName());
	}
	if (option.isAddFlag)
	{
		channel.addNewOperator(targetSocket);
		return ;
	}
	channel.removeOperator(targetSocket);
}

void ModeCmd::limitMode(Channel& channel, const ModeCmd::Option& option) const
{

	if (option.isAddFlag)
	{
		int limit = std::atoi(option.argument.c_str());
		if (channel.setLimitNbUsers(limit))
		{
			std::vector<std::string> args;
			args.push_back(channel.getName());
			args.push_back(std::string(1, option.flag));
			args.push_back(option.argument);
			throw InvalidModeParamException(args);
		}
		return ;
	}
	channel.disableLimitNbUsers();
}