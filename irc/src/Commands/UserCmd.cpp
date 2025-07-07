#include "UserCmd.hpp"

#include "User.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NotRegisteredException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"

void UserCmd::execute(const Command& cmd, const int socket) const
{
	User& user = UserManager::getInstance().getUser(socket);
	if (!user.isConnected())
		throw PasswordMismatchException(socket, user.getNickname());
	if (!user.isNickRegister())
		throw NotRegisteredException(socket, user.getNickname());
	if (cmd.arg.size() != 3 || cmd.trailing.empty())
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
		/** @todo parse "0 *" */
	if (!validUsername(cmd.arg.at(0)))
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	if (user.isUsernameRegister())
		throw AlreadyRegistredException(socket, user.getNickname());
	user.setUsername(cmd.arg.at(0));
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

