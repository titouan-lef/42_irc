#include "PingCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"

PingCmd::PingCmd(): ACommand(1, 1) {}

void PingCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::string CMD_REPLY_SERVER = "PONG";

	checkArgSize(cmd);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, CMD_REPLY_SERVER, cmd.arg);
}