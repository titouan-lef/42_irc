#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"

void PrivmsgCmd::execute(MsgServ::Command cmd, User& user) const
{
	int socket = UserManager::getInstance().findUserSocket(cmd.arg[0]);
	if (socket == -1)
		throw NoSuchNickException(user.getSocket(), cmd.arg, user.getNickname());
	MsgServ::sendMessageReply(socket, cmd, user.getPrefix());
}