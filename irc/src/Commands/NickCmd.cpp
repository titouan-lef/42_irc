#include "NickCmd.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"

void NickCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	const int sizeMax = 9;
	const int argsMax = 1;

	User &user = UserManager::getInstance().getUser(socket);
	if (!user.isConnected())
		throw PasswordMismatchException(socket, user.getNickname());
	if (!cmd.trailing.empty() || cmd.arg.size() != argsMax)
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	if (cmd.arg[0].empty())
		throw NoNicknameGivenException(socket, user.getNickname());
	if (cmd.arg[0].length() > sizeMax)
		throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	unsigned char c = cmd.arg[0][0];
	if (!isalpha(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
		throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	for (size_t i = 0; i < cmd.arg[0].length(); ++i)
	{
		c = cmd.arg[0][i];
		if (!isalnum(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
			throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	}
	if (UserManager::getInstance().findUserSocket(cmd.arg[0]) != -1)
		throw NicknameInUseException(socket, cmd.arg, user.getNickname());
	user.setNickname(cmd.arg[0]);
}
