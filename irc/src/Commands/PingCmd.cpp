#include "PingCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "NeedMoreParamsException.hpp"

void PingCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::string CMD_REPLY_SERVER = "PONG";

	checkCmd(cmd);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, CMD_REPLY_SERVER, cmd.arg, cmd.trailing);
}

void PingCmd::checkCmd(const Command& cmd) const
{
	if (cmd.arg.size() != 1 && !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
}