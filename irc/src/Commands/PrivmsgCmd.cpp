#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"

void PrivmsgCmd::execute(MsgServ::Command cmd, User & user) const
{
	(void)user;
	int socket = UserManager::getInstance().findUserSocket(cmd.arg[0]);
	if (socket == -1)
		throw NoSuchNickException(cmd.arg[0]);
	MsgServ::sendMsg(socket, cmd.trailing);
}