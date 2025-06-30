#include "PassCmd.hpp"
#include "PasswordMismatchException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"
#include "Server.hpp"
#include "UserManager.hpp"

void PassCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	const int argsMax = 1;

	User& user = UserManager::getInstance().getUser(socket);
	if (user.isConnected())
		throw AlreadyRegistredException(socket, user.getNickname());
	if (!cmd.trailing.empty() || cmd.arg.size() == 0)
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	if (cmd.arg.size() != argsMax) /** @todo check is correct exception*/
		throw PasswordMismatchException(socket);
	if (!Server::getInstance().isCorrectPassword(cmd.arg[0])) 
		throw PasswordMismatchException(socket);
	user.validConnection();
}