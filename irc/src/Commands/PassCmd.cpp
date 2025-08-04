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
 * This function handles the PASS command, which is used to provide a connection password
 * before registration is completed. If the user is already fully registered, the command
 * is rejected. Otherwise, the function checks whether the provided password matches the
 * server’s expected password.
 *
 * If the password is valid, the user's connection is marked as authenticated. If the
 * password is incorrect, a `PasswordMismatchException` is thrown.
 *
 * @param cmd The command object containing the PASS request and password as the last parameter.
 * @param socket The socket descriptor of the user attempting to authenticate.
 *
 * @throws AlreadyRegistredException If the user is already registered.
 * @throws PasswordMismatchException If the provided password does not match the server’s password.
 */
void PassCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkCmd(cmd);
	User& user = UserManager::getInstance().getUser(senderSocket);
	user.passToNextConnectionStep();
}

void PassCmd::checkCmd(const Command& cmd) const
{
	checkArgSize(cmd);
	const std::string& password = cmd.arg.at(0);
	if (Server::getInstance().isInvalidPassword(password))
		throw PasswordMismatchException();
}
