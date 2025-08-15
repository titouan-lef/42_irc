#include "PingCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "MsgServ.hpp"

PingCmd::PingCmd(): ACommand(CONNECTED, CONNECTED, 1, 1) {}

/**
 * @brief Executes the PING command to test the presence of an active client.
 * 
 * @param cmd The command object containing the ping request and its arguments.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 */
void PingCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	const std::string CMD_REPLY_SERVER = "PONG";

	checkUserStatus(userStatus);
	checkArgSize(cmd);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, CMD_REPLY_SERVER, cmd.arg);
}