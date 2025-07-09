#include "PassCmd.hpp"

#include "PasswordMismatchException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"
#include "Server.hpp"
#include "UserManager.hpp"

#define ARG_REQUIERED 1

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
