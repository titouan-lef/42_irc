#include "PartCmd.hpp"

#include <vector>

#include "command.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "IrcException.hpp"

/**
 * @brief Executes the PART command to remove a user from one or more channels.
 *
 * This function processes the PART command, which allows a user to leave one or more channels.
 * The command must include exactly one argument containing a comma-separated list of channel names.
 * An optional parting message can be included in the trailing portion of the command.
 *
 * For each specified channel, the function attempts to remove the user (identified by the socket)
 * and send the optional message. If an error occurs (e.g., the channel does not exist or the user
 * is not a member), the corresponding IRC error is sent back to the user.
 *
 * @param cmd The command object containing the PART request, its arguments, and optional message.
 * @param socket The socket descriptor of the user issuing the PART command.
 *
 * @throws NeedMoreParamsException If the command does not contain exactly one argument.
 */
void PartCmd::execute(const Command& cmd, const int senderSocket) const
{
	checkCmd(cmd);
	partChannelOfCmd(cmd, senderSocket);
}

void PartCmd::checkCmd(const Command& cmd) const
{
	const std::size_t NB_ARG_MIN = 1;
	const std::size_t NB_ARG_MAX = 2;

	if (cmd.arg.size() < NB_ARG_MIN || cmd.arg.size() > NB_ARG_MAX)
		throw NeedMoreParamsException(cmd.name);
}

void PartCmd::partChannelOfCmd(const Command &cmd, const int senderSocket) const
{
	const char* message = (cmd.arg.size() == 2 ? cmd.arg.at(1).c_str() : NULL);
	const std::vector<std::string> channelList = splitTargets(cmd.arg.at(0));
	for (size_t i = 0; i < channelList.size(); ++i)
		partOneChannel(message, senderSocket, channelList.at(i));
}

void PartCmd::partOneChannel(const char* message, const int senderSocket, const std::string& channelName) const
{
	try
	{
		ChanServ::getInstance().part(channelName, senderSocket, message);
	}
	catch (const IrcException& e)
	{
		e.sendError(senderSocket);
	}
}
