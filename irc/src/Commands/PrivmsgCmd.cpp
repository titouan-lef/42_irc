#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"
#include "NeedMoreParamsException.hpp"

/** @todo penser a parser le nom de user */
void PrivmsgCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 1)
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	int socketReceiver = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	if (socketReceiver == -1)
		throw NoSuchNickException(socket, cmd.arg.at(0));
	MsgServ::getInstance().sendMessageReply(socketReceiver, cmd);
}