#include "InviteCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"

InviteCmd::InviteCmd(): ACommand(CONNECTED, CONNECTED, 2, 2) {}

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
void InviteCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	const std::string& nickname = cmd.arg.at(0);
	const int invitedSocket = UserManager::getInstance().findUserSocket(nickname);
	const std::string& channelName = cmd.arg.at(1);
	inviteUserInChannel(cmd, senderSocket, channelName, invitedSocket);
}

void InviteCmd::inviteUserInChannel(const Command& cmd, const int senderSocket, const std::string& channelName, const int invitedSocket) const
{
	Channel& channel = ChanServ::getInstance().findChannel(channelName);
	channel.inviteUser(invitedSocket, senderSocket);

	MsgServ::getInstance().sendMessageReply(senderSocket, invitedSocket, cmd);
}
