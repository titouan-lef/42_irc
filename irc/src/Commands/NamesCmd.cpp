#include "NamesCmd.hpp"

#include "ChanServ.hpp"

#define ARG_REQUIERED 1

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
void NamesCmd::execute(const Command& cmd, const int socket) const
{
	if (getNbArgs(cmd) == 0)
	{
		ChanServ::getInstance().namesAllChannels(socket);
		return;
	}
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	std::vector<std::string> channels = split(lastParam, ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		ChanServ::getInstance().findChannel(channels.at(i)).sendActualUsers(socket);
	}
}
