#include "InviteCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"

/**
 * @brief Executes the INVITE command to invite a user to a channel.
 *
 * This function handles the logic for inviting a user to a channel.
 * finds the socket associated with the user to be invited, and then instructs
 * the channel service to invite the user to the specified channel.
 *
 * @param cmd The command object containing the invite request and its arguments.
 * @param senderSocket The socket descriptor of the user sending the invite.
 */
void InviteCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::string channelName = checkCmd(cmd);
	const int invitedSocket = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	inviteUserInChannel(cmd, senderSocket, channelName, invitedSocket);
}

const std::string InviteCmd::checkCmd(const Command &cmd) const
{
	const std::size_t NB_ARG = 2;

	return getLastParam(cmd, NB_ARG);
}

void InviteCmd::inviteUserInChannel(const Command& cmd, const int senderSocket, const std::string& channelName, const int invitedSocket) const
{
	Channel& channel = ChanServ::getInstance().findChannel(channelName);
	channel.inviteUser(invitedSocket, senderSocket);

	MsgServ::getInstance().sendMessageReply(senderSocket, invitedSocket, cmd.name, cmd.arg, cmd.trailing);
}
