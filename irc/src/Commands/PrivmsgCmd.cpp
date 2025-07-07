#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"
#include "NeedMoreParamsException.hpp"

/** @todo penser a parser le nom de user */
void PrivmsgCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() < 1)
		throw NeedMoreParamsException(cmd.name);
	int receiverSocket = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	if (receiverSocket == -1)
		throw NoSuchNickException(cmd.arg.at(0));
	MsgServ::getInstance().sendMessageReply(senderSocket, receiverSocket, cmd.name, cmd.trailing);
}