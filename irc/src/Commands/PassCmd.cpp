#include "PassCmd.hpp"

#include "PasswordMismatchException.hpp"
#include "Server.hpp"
#include "UserManager.hpp"
#include "User.hpp"
#include "command.hpp"
#include "status.hpp"

PassCmd::PassCmd(): ACommand(DISCONNECTED, DISCONNECTED, 1, 1) {}

/**
 * @brief Executes the PASS command to authenticate a user with the server password.
 * 
 * @param cmd The command object containing the pass request and password as the last parameter.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throws Command: PasswordMismatchException
 */
void PassCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkCmd(cmd);
	User& user = UserManager::getInstance().findUser(senderSocket);
	user.passToNextConnectionStep();
}

void PassCmd::checkCmd(const Command& cmd) const
{
	checkArgSize(cmd);
	const std::string& password = cmd.arg.at(0);
	if (Server::getInstance().isInvalidPassword(password))
		throw PasswordMismatchException();
}
