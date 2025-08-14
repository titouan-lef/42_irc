#include "PartCmd.hpp"

#include <vector>

#include "command.hpp"
#include "status.hpp"
#include "ChanServ.hpp"
#include "IrcException.hpp"

PartCmd::PartCmd(): ACommand(CONNECTED, CONNECTED, 1, 2) {}

/**
 * @brief Executes the PART command to remove a user from one or more channels.
 *
 * @param cmd The command object containing the part request, its arguments, and optional message.
 * @param socket The socket descriptor of the user issuing the PART command.
 * @param userStatus The sender user status.
 * 
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throws Command: NeedMoreParamsException, NoSuchChannelException
 */
void PartCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	partChannelOfCmd(cmd, senderSocket);
}

void PartCmd::partChannelOfCmd(const Command &cmd, const int senderSocket) const
{
	const char* message = (cmd.arg.size() == nbArgMax_ ? cmd.arg.at(1).c_str() : NULL);
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
