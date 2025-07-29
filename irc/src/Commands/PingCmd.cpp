#include "PingCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "NeedMoreParamsException.hpp"

void PingCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::string CMD_REPLY_SERVER = "PONG";

	checkCmd(cmd);
	MsgServ::getInstance().sendMessageReply(senderSocket, senderSocket, CMD_REPLY_SERVER, cmd.arg);
}

void PingCmd::checkCmd(const Command& cmd) const
{
	const std::size_t NB_ARG = 1;

	if (cmd.arg.size() != NB_ARG)
		throw NeedMoreParamsException(cmd.name);
}