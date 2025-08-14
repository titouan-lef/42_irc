#include "PongCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "UserManager.hpp"

PongCmd::PongCmd(): ACommand(CONNECTED, CONNECTED, 1, 1) {}

/**
 * @brief Executes the PONG command for reply to ping message.
 * 
 * @param cmd The command object containing the pong request and password as the last parameter.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 */
void PongCmd::execute(const Command& cmd, const int socketSender, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	UserManager::getInstance().validatePongResponse(socketSender);
	UserManager::getInstance().updateLastActivity(socketSender);
}