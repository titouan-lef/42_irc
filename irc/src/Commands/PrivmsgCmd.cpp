#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"

/** @todo penser a parser le nom de user */
void PrivmsgCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	int socketReceiver = UserManager::getInstance().findUserSocket(cmd.arg[0]);
	if (socketReceiver == -1)
		throw NoSuchNickException(socket, cmd.arg[0]);
	MsgServ::getInstance().sendMessageReply(socketReceiver, cmd);
}