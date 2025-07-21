#include "ListCmd.hpp"

#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "IrcException.hpp"

#define ARG_REQUIERED 1

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
 * @param socket The socket descriptor of the user requesting the channel list.
 */
void ListCmd::execute(const Command& cmd, const int socket) const
{
	if (getNbArgs(cmd) == 0)
	{
		ChanServ::getInstance().listAllChannels(socket);
		return;
	}
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	std::vector<std::string> channels = split(lastParam, ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		try
		{
			ChanServ::getInstance().findChannel(channels.at(i)).sendChannelSummary(socket);
		}
		catch(const IrcException& e)
		{
			e.sendError(socket);
		}
		
	}
	MsgServ::getInstance().sendNumericReply(socket, 323, "End of LIST");
}