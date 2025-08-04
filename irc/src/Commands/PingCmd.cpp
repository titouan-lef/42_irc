#include "PingCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "MsgServ.hpp"

PingCmd::PingCmd(): ACommand(CONNECTED, CONNECTED, 1, 1) {}

void PingCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	const std::string CMD_REPLY_SERVER = "PONG";

	checkUserStatus(userStatus);
	checkArgSize(cmd);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, CMD_REPLY_SERVER, cmd.arg);
}