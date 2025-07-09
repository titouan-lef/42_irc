#include "InviteCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

#define ARG_REQUIERED 1

void InviteCmd::execute(const Command& cmd, const int senderSocket) const
{
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	const int invatedSocket = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	ChanServ::getInstance().findChannel(lastParam).inviteUser(invatedSocket, senderSocket);
}