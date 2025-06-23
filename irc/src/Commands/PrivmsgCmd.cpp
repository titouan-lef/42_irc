#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"

void PrivmsgCmd::execute(std::vector<std::string> args, std::string trailing, const User & user) const
{
	(void)user;
	int socket = UserManager::getInstance().findUserSocket(args[0]);
	if (socket == -1)
		throw NoSuchNickException(args[0]);
	MsgServ::sendMsg(socket, trailing);
}