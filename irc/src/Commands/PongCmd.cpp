#include "PongCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "UserManager.hpp"

PongCmd::PongCmd(): ACommand(CONNECTED, CONNECTED, 1, 1) {}

void PongCmd::execute(const Command& cmd, const int socketSender, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	UserManager::getInstance().validatePongResponse(socketSender);
	UserManager::getInstance().updateLastActivity(socketSender);
}