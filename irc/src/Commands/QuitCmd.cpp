#include "QuitCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"

void QuitCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	UserManager::getInstance().closeConnectionBySocket(socket);
	if (!cmd.trailing.empty())
	{
		std::string msg = cmd.trailing.substr(1);
	}
}