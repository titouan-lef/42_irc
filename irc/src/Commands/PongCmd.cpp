#include "PongCmd.hpp"

#include "command.hpp"
#include "UserManager.hpp"

PongCmd::PongCmd(): ACommand(1, 1) {}

void PongCmd::execute(const Command& cmd, const int socketSender) const
{
	checkArgSize(cmd);
	UserManager::getInstance().validatePongResponse(socketSender);
	UserManager::getInstance().updateLastActivity(socketSender);
}