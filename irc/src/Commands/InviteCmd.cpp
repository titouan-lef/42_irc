#include "InviteCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void InviteCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() != 2 || !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	const int invatedSocket = UserManager::getInstance().findUserSocket(cmd.arg.at(0));
	ChanServ::getInstance().findChannel(cmd.arg.at(1)).inviteUser(invatedSocket, senderSocket);
}