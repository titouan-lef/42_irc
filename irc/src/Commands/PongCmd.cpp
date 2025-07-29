#include "PongCmd.hpp"

#include "command.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void PongCmd::execute(const Command& cmd, const int socketSender) const
{
	checkCmd(cmd);
	UserManager::getInstance().validatePongResponse(socketSender);
	UserManager::getInstance().updateLastActivity(socketSender);
}

void PongCmd::checkCmd(const Command& cmd) const
{
	const std::size_t NB_ARG = 1;

	if (cmd.arg.size() != NB_ARG)
		throw NeedMoreParamsException(cmd.name);
}