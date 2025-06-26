#include "UserCmd.hpp"
#include "User.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"

void UserCmd::execute(MsgServ::Command cmd, User& user) const
{
	if (cmd.arg.size() != 3 || cmd.trailing.empty())
		throw NeedMoreParamsException(user.getSocket(), cmd.name, user.getNickname());
	if (!validUsername(cmd.arg[0]))
		throw NeedMoreParamsException(user.getSocket(), cmd.name, user.getNickname());
	if (!user.isUsernameRegister())
		throw AlreadyRegistredException(user.getSocket(), user.getNickname());
	user.setUsername(cmd.arg[0]);
	user.setRealName(cmd.trailing);
}

/** @todo tcheck  */
bool UserCmd::validUsername(std::string username) const
{
	for (size_t i = 0; i < username.length(); ++i)
	{
		unsigned char c = username[i];
		if (!isalnum(c))
			return false;
	}
	return true;
}

