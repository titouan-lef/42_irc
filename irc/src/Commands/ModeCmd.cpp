#include "ModeCmd.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoSuchChannelException.hpp"
#include "NoSuchNickException.hpp"
#include "NoSuchNickException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InvalidModeParamException.hpp"
#include <cstdlib>
#include "ChannelPrivilegesNeeded.hpp"
#include "InvalidModeParamException.hpp"
#include <cstdlib>

void ModeCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 1)
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	std::string channelName = cmd.arg.at(0);
	if (channelName.at(0) != '#')
		throw NoSuchChannelException(socket, channelName.substr(1));
	Channel& channel = ChanServ::getInstance().findChannel(channelName.substr(1), socket);
	if (!channel.isModerator(socket))
		throw ChannelPrivilegesNeeded(socket, channel.getName());
	if (cmd.arg.size() == 1)
	{
		sendActualModes(channel, socket);
		return;
	}
	std::vector<Option> options = parseCommand(cmd, socket);
	executeModeOptions(channel, options, socket);
}
/** @todo MODE #channel -> ask for actives modes 324 → numeric code for RPL_CHANNELMODEIS*/ 

void ModeCmd::sendActualModes(Channel& channel, const int socket) const
{
	std::vector<std::string> arg;

	arg.push_back("#" + channel.getName());
	arg.push_back(channel.builtOptionString());
	MsgServ::Command cmd = {"324", "", arg};
	MsgServ::getInstance().sendNumericReply(socket, cmd, UserManager::getInstance().getUserNickname(socket));
}


std::vector<ModeCmd::Option> ModeCmd::parseCommand(const MsgServ::Command& cmd, const int socket) const
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

void ModeCmd::addOptionInList(const MsgServ::Command &cmd, const int socket, std::vector<Option> &optionList, argIterator& it) const
{
	const std::string& flag = *it;
	if ((flag[0] != '+' && flag[0] != '-') || flag[1] == '\0')
		throw UnknownModeException(socket, flag[0]);
	const bool isAddFlag = (flag[0] == '+');
	for (std::size_t i = 1; flag[i] != '\0'; ++i)
	{
		Option option = {flag[i], isAddFlag, ""};
		checkOptionAndAddArg(cmd, socket, it, option);
		optionList.push_back(option);
	}
}

void ModeCmd::checkOptionAndAddArg(const MsgServ::Command& cmd, const int socket, argIterator& it, Option& option) const
{
	const std::string validFlag = "itokl";
	const std::size_t posFlagArg = 2;

	std::size_t pos = validFlag.find(option.flag);
	if (pos == std::string::npos)
		throw UnknownModeException(socket, option.flag);
	if (pos == posFlagArg || (pos > posFlagArg && option.isAddFlag))
		option.argument = defineOptionArg(cmd, socket, it);
}

const std::string ModeCmd::defineOptionArg(const MsgServ::Command& cmd, const int socket, argIterator& it) const
{
	++it;
	if (it >= cmd.arg.end())
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	return *it;
}

void ModeCmd::executeModeOptions(Channel& channel, const std::vector<ModeCmd::Option>& options, const int socket) const
{
	for (std::vector<Option>::const_iterator it = options.begin() ; it < options.end(); ++it)
	{
		try
		{
			executeModeOption(channel, *it, socket);
		}
		catch(const IrcException& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}

void ModeCmd::executeModeOption(Channel& channel, const ModeCmd::Option& option, const int socket) const
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
			operatorMode(channel, option, socket);
			break;
		case 'l':
			limitMode(channel, option, socket);
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

void ModeCmd::operatorMode(Channel& channel, const ModeCmd::Option& option, const int socket) const
{
	int targetSocket = UserManager::getInstance().findUserSocket(option.argument);
	if (targetSocket == -1)
		throw NoSuchNickException(socket, option.argument);
	if (option.isAddFlag)
	{
		channel.addNewModerator(targetSocket);
		return ;
	}
	channel.removeModerator(targetSocket);
}

void ModeCmd::limitMode(Channel& channel, const ModeCmd::Option& option, const int socket) const
{

	if (option.isAddFlag)
	{
		int limit = std::atoi(option.argument.c_str());
		if (channel.setLimitNbUsers(limit))
		{
			std::vector<std::string> args;
			args.push_back("#" + channel.getName());
			args.push_back(std::string(1, option.flag));
			args.push_back(option.argument);
			throw InvalidModeParamException(socket, args, UserManager::getInstance().getUserNickname(socket));
		}
		return ;
	}
	channel.disableLimitNbUsers();
}