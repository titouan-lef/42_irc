#include "UserCmd.hpp"

#include "User.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NotRegisteredException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"

void UserCmd::execute(const Command& cmd, const int senderSocket) const
{
	User& user = UserManager::getInstance().getUser(senderSocket);
	if (!user.isConnected())
		throw PasswordMismatchException();
	if (!user.isNickRegister())
		throw NotRegisteredException();
	if (cmd.arg.size() != 3 || cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	if (!validParam(cmd.arg))
		throw NeedMoreParamsException(cmd.name); /** @todo parse "0 *" */
	if (!validUsername(cmd.arg.at(0)))
		throw NeedMoreParamsException(cmd.name);
	if (user.isUsernameRegister())
		throw AlreadyRegistredException();
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

/**
 * @brief Verifie if the mode are "0" and "*" if is not return false
 */
bool UserCmd::validParam(const std::vector<std::string>&  param) const
{
	if (param.at(1) != "0")
		return false;
	if (param.at(2) != "*")
		return false;
	return true;
}

