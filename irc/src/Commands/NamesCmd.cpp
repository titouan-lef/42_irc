#include "NamesCmd.hpp"

#include "ChanServ.hpp"
#include "command.hpp"
#include "status.hpp"

NamesCmd::NamesCmd(): ACommand(CONNECTED, CONNECTED, 0, 1) {}

/**
 * @brief Executes the NAMES command to list users in specified channels or all channels.
 *
 * @param cmd The command object containing the NAMES request and its arguments.
 * @param socket The socket descriptor of the user requesting the user lists.
 * @param userStatus The sender user status.
 * 
 * @throw Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
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
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	channel.sendActualUsers(senderSocket);
}
