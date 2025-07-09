#include "PassCmd.hpp"

#include "PasswordMismatchException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"
#include "Server.hpp"
#include "UserManager.hpp"

void PassCmd::execute(const Command& cmd, const int socket) const
{
	const int argsMax = 1;

	User& user = UserManager::getInstance().getUser(socket);
	if (user.isConnected())
		throw AlreadyRegistredException();
	if (cmd.trailing.empty())
	{
		if (cmd.arg.size() != argsMax) /** @todo check is correct exception*/
			throw NeedMoreParamsException(cmd.name);
		if (!Server::getInstance().isCorrectPassword(cmd.arg.at(0))) 
			throw PasswordMismatchException();
		user.validConnection();
	}
	else
	{
		if (cmd.arg.size() != 0) /** @todo check is correct exception*/
			throw NeedMoreParamsException(cmd.name);
		if (!Server::getInstance().isCorrectPassword(cmd.trailing))
			throw PasswordMismatchException();
		user.validConnection();
	}
}
