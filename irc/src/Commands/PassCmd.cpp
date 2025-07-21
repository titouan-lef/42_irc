#include "PassCmd.hpp"

#include "PasswordMismatchException.hpp"
#include "AlreadyRegistredException.hpp"
#include "Server.hpp"
#include "UserManager.hpp"
#include "User.hpp"

#define ARG_REQUIERED 1

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
void PassCmd::execute(const Command& cmd, const int socket) const
{
	User& user = UserManager::getInstance().getUser(socket);
	if (user.isConnected())
		throw AlreadyRegistredException();
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	if (!Server::getInstance().isCorrectPassword(lastParam))
			throw PasswordMismatchException();
	user.validConnection();
}
