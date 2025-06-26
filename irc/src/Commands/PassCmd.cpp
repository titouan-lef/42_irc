#include "PassCmd.hpp"
#include "PasswordMismatchException.hpp"
#include "Server.hpp"

void PassCmd::execute(MsgServ::Command cmd, User& user) const
{
	const int argsMax = 1;

	if (!cmd.trailing.empty() || cmd.arg.size() != argsMax || cmd.arg[0].empty())
		throw PasswordMismatchException(user.getSocket(), user.getNickname());
	if (!Server::getInstance().isCorrectPassword(cmd.arg[0])) 
		throw PasswordMismatchException(user.getSocket(), user.getNickname());
	user.validConnection();
}