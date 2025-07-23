#include "NamesCmd.hpp"

#include "ChanServ.hpp"

/**
 * @brief Executes the NAMES command to list users in specified channels or all channels.
 *
 * If no arguments are provided in the command, the function requests the names of users
 * in all available channels and sends them to the requesting user.
 * If arguments are present, it treats them as a comma-separated list of channel names,
 * retrieves each specified channel, and sends the list of current users in that channel
 * to the user identified by the given socket.
 *
 * @param cmd The command object containing the NAMES request and its arguments.
 * @param socket The socket descriptor of the user requesting the user lists.
 */
void NamesCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::size_t NB_ARG_ALL_CHAN = 0;

	if (getNbArg(cmd) == NB_ARG_ALL_CHAN)
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
	const std::size_t NB_ARG = 1;

	const std::string lastParam = getLastParam(cmd, NB_ARG);
	std::vector<std::string> channelList = splitTargets(lastParam);
	for (size_t i = 0; i < channelList.size(); ++i)
		sendListUserInOneChannel(senderSocket, channelList.at(i));
}

void NamesCmd::sendListUserInOneChannel(const int senderSocket, const std::string& channelName) const
{
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	channel.sendActualUsers(senderSocket);
}
