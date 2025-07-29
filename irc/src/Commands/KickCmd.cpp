#include "KickCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "ChannelPrivilegesNeeded.hpp"

void KickCmd::execute(const Command& cmd, const int senderSocket) const
{
	checkCmd(cmd);

	std::vector<std::string> channelList = splitTargets(cmd.arg.at(0));
	std::vector<std::string> userList = splitTargets(cmd.arg.at(1));
	checkCmdArg(cmd, channelList.size(), userList.size());

	const std::string message = buildMessage(cmd, senderSocket);

	for (std::size_t i = 0; i < userList.size(); ++i)
	{
		const std::string& channelName = getChannelName(i, channelList);
		kickOneChannel(senderSocket, channelName, userList.at(i), message);
	}
}

void KickCmd::checkCmd(const Command& cmd) const
{
	const std::size_t NB_ARG_MIN = 2;
	const std::size_t NB_ARG_MAX = 3;

	if (cmd.arg.size() < NB_ARG_MIN || cmd.arg.size() > NB_ARG_MAX)
		throw NeedMoreParamsException(cmd.name);
}

void KickCmd::checkCmdArg(const Command& cmd, const std::size_t nbChannel, const std::size_t nbUser) const
{
	const std::size_t NB_CHAN_ALWAYS_OK = 1;

	if (nbChannel == NB_CHAN_ALWAYS_OK)
		return;
	if (nbChannel != nbUser)
		throw NeedMoreParamsException(cmd.name);
}

const std::string KickCmd::buildMessage(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() == 3)
		return cmd.arg.at(2);
	const std::string& nickname = UserManager::getInstance().getUserNickname(senderSocket);
	return ("Kicked by " + nickname);
}

const std::string& KickCmd::getChannelName(const std::size_t userIndex, const std::vector<std::string>& channelList) const
{
	const std::size_t channelIndex = (userIndex >= channelList.size() ? 0 : userIndex);
	return channelList.at(channelIndex);
}

void KickCmd::kickOneChannel(const int senderSocket, const std::string& channelName, const std::string& nickname, const std::string& message) const
{
	try
	{
		checkChannel(senderSocket, channelName);
		int socketKicked = UserManager::getInstance().findUserSocket(nickname);
		ChanServ::getInstance().kick(channelName, socketKicked, senderSocket, message);
	}
	catch (const IrcException& e)
	{
		e.sendError(senderSocket);
	}
}

void KickCmd::checkChannel(const int senderSocket, const std::string& channelName) const
{
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	if (!channel.isOperator(senderSocket))
		throw ChannelPrivilegesNeeded(channelName);
}
