#include "ListCmd.hpp"

#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "IrcException.hpp"
#include "NeedMoreParamsException.hpp"
#include "command.hpp"

/**
 * @brief Executes the LIST command to display available channels or specific channel summaries.
 *
 * If no arguments are provided in the command, the function lists all available channels to the user.
 * If arguments are present, it assumes they represent a comma-separated list of channel names,
 * and attempts to send a summary of each specified channel to the requesting user.
 *
 * In case a specified channel is not found or an error occurs, an appropriate error message is sent
 * back through the socket using the exception mechanism.
 * After processing, a numeric reply (code 323) is sent to indicate the end of the list.
 *
 * @param cmd The command object containing the LIST request and its arguments.
 * @param senderSocket The socket descriptor of the user requesting the channel list.
 */
void ListCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::size_t NB_ARG_ALL_CHAN = 0;

	checkCmd(cmd);
	if (cmd.arg.size() == NB_ARG_ALL_CHAN)
		sendInfoAllChannel(senderSocket);
	else
		sendInfoSpecificChannel(cmd, senderSocket);
}

void ListCmd::checkCmd(const Command &cmd) const
{
	const std::size_t NB_ARG_MAX = 1;

	if (cmd.arg.size() > NB_ARG_MAX)
		throw NeedMoreParamsException(cmd.name);
}

void ListCmd::sendInfoAllChannel(const int senderSocket) const
{
	ChanServ::getInstance().listAllChannels(senderSocket);
}

void ListCmd::sendInfoSpecificChannel(const Command &cmd, const int senderSocket) const
{
	std::vector<std::string> channelList = splitTargets(cmd.arg.at(0));
	for (size_t i = 0; i < channelList.size(); ++i)
		sendInfoOneChannel(senderSocket, channelList.at(i));
	MsgServ::getInstance().sendNumericReply(senderSocket, 323, "End of LIST");
}

void ListCmd::sendInfoOneChannel(const int senderSocket, const std::string &channelName) const
{
	try
	{
		Channel& channel = ChanServ::getInstance().findChannel(channelName);
		channel.sendChannelSummary(senderSocket);
	}
	catch(const IrcException& e)
	{
		e.sendError(senderSocket);
	}
}
