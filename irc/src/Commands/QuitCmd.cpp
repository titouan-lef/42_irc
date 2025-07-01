#include "QuitCmd.hpp"
#include "UserManager.hpp"

void QuitCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	UserManager::getInstance().closeConnectionBySocket(socket);
	if (!cmd.trailing.empty())
	{
		/** comment savoir dans quell channel je suis */
	}
}