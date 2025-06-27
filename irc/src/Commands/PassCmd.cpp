#include "PassCmd.hpp"
#include "PasswordMismatchException.hpp"
#include "Server.hpp"
#include "NeedMoreParamsException.hpp"

void PassCmd::execute(MsgServ::Command cmd, User& user) const
{
	const int argsMax = 1;

	if (!cmd.trailing.empty() || cmd.arg[0].empty())
		throw NeedMoreParamsException(user.getSocket(), cmd.name, user.getNickname());
	if (cmd.arg.size() != argsMax)
		throw PasswordMismatchException(user.getSocket(), user.getNickname());
	if (!Server::getInstance().isCorrectPassword(cmd.arg[0])) 
		throw PasswordMismatchException(user.getSocket(), user.getNickname());
	user.validConnection();
}