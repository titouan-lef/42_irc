#include "ModeCmd.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoSuchChannelException.hpp"
#include "UnknownModeException.hpp"

void ModeCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 2)
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	std::string channelName = cmd.arg.at(0);
	if (channelName.at(0) != '#')
		throw NoSuchChannelException(socket, channelName.substr(1));
	Channel& channel = ChanServ::getInstance().findChannel(channelName.substr(1), socket);
	std::vector<Option> options = parseCommand(cmd, socket);
	(void)channel;//next to come
}
/** @todo MODE #channel -> ask for actives modes 324 → numeric code for RPL_CHANNELMODEIS*/ 

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