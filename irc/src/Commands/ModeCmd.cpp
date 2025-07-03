#include "ModeCmd.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoSuchChannelException.hpp"
#include "NoSuchNickException.hpp"
#include "UnknownModeException.hpp"
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
	size_t argIndex = 2;
	std::vector<Option> options;
	std::string modeString = cmd.arg.at(1);
	int setOrRemove = -1;
	for (std::string::iterator it = modeString.begin() ; it < modeString.end(); ++it)
	{
		try
		{
			processModeChar(*it, setOrRemove, argIndex, cmd, socket, options);
		}
		catch(const UnknownModeException& e)
		{
			e.sendError();
		}
	}
	return options;
}

void ModeCmd::processModeChar(char modeChar, int& setOrRemove, size_t& argIndex,
							  const MsgServ::Command& cmd, int socket,
							  std::vector<ModeCmd::Option>& options) const
{
	Option option = {modeChar, setOrRemove, ""};
	switch (modeChar)
	{
		case '+':
			setOrRemove = true;
			break;
		case '-':
			setOrRemove = false;
			break;
		case 'i':
		case 't':
			if (setOrRemove == -1)
				throw UnknownModeException(socket, modeChar);
			options.push_back(option);
			break;
		case 'k':
		case 'o':
		case 'l':
			if (setOrRemove == -1)
				throw UnknownModeException(socket, modeChar);
			if (argIndex >= cmd.arg.size())
				throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
			option.argument = cmd.arg.at(argIndex);
			options.push_back(option);
			++argIndex;
			break;
		default:
			throw UnknownModeException(socket, modeChar);
	}
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
	switch (option.type)
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
	if (option.setOrRemove)
	{
		channel.enableInviteRestriction();
		return ;
	}
	channel.disableInviteRestriction();
}

void ModeCmd::topicMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.setOrRemove)
	{
		channel.enableTopicRestriction();
		return ;
	}
	channel.disableTopicRestriction();
}

void ModeCmd::passwordMode(Channel& channel, const ModeCmd::Option& option) const
{
	if (option.setOrRemove)
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
	if (option.setOrRemove)
	{
		channel.addNewModerator(targetSocket);
		return ;
	}
	channel.removeModerator(targetSocket);
}

void ModeCmd::limitMode(Channel& channel, const ModeCmd::Option& option, const int socket) const
{

	if (option.setOrRemove)
	{
		int limit = std::atoi(option.argument.c_str());
		if (channel.setLimitNbUsers(limit))
		{
			std::vector<std::string> args;
			args.push_back("#" + channel.getName());
			args.push_back(std::string(1, option.type));
			args.push_back(option.argument);
			throw InvalidModeParamException(socket, args, UserManager::getInstance().getUserNickname(socket));
		}
		return ;
	}
	channel.disableLimitNbUsers();
}