#include "NamesCmd.hpp"

#include "ChanServ.hpp"
#include "IrcException.hpp"
#include "command.hpp"
#include "status.hpp"

NamesCmd::NamesCmd(): ACommand(CONNECTED, CONNECTED, 0, 1) {}

/**
 * @brief Executes the NAMES command to list users in specified channels or all channels.
 *
 * @param cmd The command object containing the names request and its arguments.
 * @param socket The socket descriptor of the user requesting the user lists.
 * @param userStatus The sender user status.
 * 
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throws Command: NoSuchChannelException.
 */
void NamesCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	if (cmd.arg.size() == nbArgMin_)
		sendListUserInEachChannel(senderSocket);
	else
		sendListUserInSpecificChannel(cmd, senderSocket);
}

void NamesCmd::sendListUserInEachChannel(const int senderSocket) const
{
	ChanServ::getInstance().namesAllChannels(senderSocket);
}

void NamesCmd::sendListUserInSpecificChannel(const Command &cmd, const int senderSocket) const
{
	std::vector<std::string> channelList = splitTargets(cmd.arg.at(0));
	for (size_t i = 0; i < channelList.size(); ++i)
		sendListUserInOneChannel(senderSocket, channelList.at(i));
}

void NamesCmd::sendListUserInOneChannel(const int senderSocket, const std::string& channelName) const
{
	try
	{
		const Channel& channel = ChanServ::getInstance().findChannel(channelName);
		channel.sendActualUsers(senderSocket);
	}
	catch(const IrcException& e)
	{
		e.sendError(senderSocket);
	}
}
