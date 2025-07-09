#include "PingCmd.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void PingCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() != 1 && !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, "PONG", cmd.arg, cmd.trailing);
}