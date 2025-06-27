#include "NickCmd.hpp"
#include "UnknowCmdException.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"

/**
 * std::string::npos est egale "Pas trouvé" / "valeur invalide" dans une recherche
 */
void NickCmd::execute(MsgServ::Command cmd, User& user) const
{
	const int sizeMax = 9;
	const int argsMax = 1;

	if (!cmd.trailing.empty() || cmd.arg.size() != argsMax)
		throw NeedMoreParamsException(user.getSocket(), cmd.name, user.getNickname());
	if (cmd.arg[0].empty())
		throw NoNicknameGivenException(user.getSocket(), user.getNickname());
	if (cmd.arg[0].length() > sizeMax)
		throw ErroneUsNicknameException(user.getSocket(), cmd.arg, user.getNickname());
	unsigned char c = cmd.arg[0][0];
	if (!isalpha(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
		throw ErroneUsNicknameException(user.getSocket(), cmd.arg, user.getNickname());
	for (size_t i = 0; i < cmd.arg[0].length(); ++i)
	{
		c = cmd.arg[0][i];
		if (!isalnum(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
			throw ErroneUsNicknameException(user.getSocket(), cmd.arg, user.getNickname());
	}
	if (UserManager::getInstance().findUserSocket(cmd.arg[0]) != -1)
		throw NicknameInUseException(user.getSocket(), cmd.arg, user.getNickname());
	user.setNickname(cmd.arg[0]);
}
