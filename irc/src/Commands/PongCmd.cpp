#include "PongCmd.hpp"

#include "command.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void PongCmd::execute(const Command& cmd, const int socketSender) const
{
	if (cmd.arg.size() != 1 && !cmd.trailing.empty()) /** @todo necessary ?? */
		throw NeedMoreParamsException(cmd.name);
	UserManager::getInstance().validatePongResponse(socketSender);
	UserManager::getInstance().updateLastActivity(socketSender);
}