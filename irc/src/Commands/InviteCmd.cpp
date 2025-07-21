#include "InviteCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"

#define ARG_REQUIERED 2

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
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	const int invitedSocket = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	ChanServ::getInstance().findChannel(lastParam).inviteUser(invitedSocket, senderSocket);
	MsgServ::getInstance().sendMessageReply(senderSocket, invitedSocket, cmd.name, cmd.arg, cmd.trailing);
}