#include "PartCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

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
void PartCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.size() != 1)
		throw NeedMoreParamsException(cmd.name);
	std::string message;
	if (!cmd.trailing.empty())
		message = cmd.trailing.substr(1);
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		try
		{
			ChanServ::getInstance().part(channels.at(i), socket, message);
		}
		catch (const IrcException& e)
		{
			e.sendError(socket);
		}
	}
}
