#include "NickCmd.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"

#define MAX_NICK_LENGTH 9
#define MAX_ARGS 1

void NickCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	User &user = UserManager::getInstance().getUser(socket);
	if (!user.isConnected())
		throw PasswordMismatchException(socket, user.getNickname());
	if (!cmd.trailing.empty() || cmd.arg.size() != MAX_ARGS)
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	if (cmd.arg[0].empty())
		throw NoNicknameGivenException(socket, user.getNickname());
	if (cmd.arg[0].length() > MAX_NICK_LENGTH)
		throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	std::string specials = "_[]\\`^{}|";
	unsigned char c = cmd.arg[0][0];
	if ((!isalpha(c)) && specials.find(c) == std::string::npos)
		throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	for (size_t i = 1; i < cmd.arg[0].length(); ++i)
	{
		c = cmd.arg[0][i];
		if ((!isalnum(c)) && specials.find(c) == std::string::npos && c != '-')
			throw ErroneUsNicknameException(socket, cmd.arg, user.getNickname());
	}
	if (UserManager::getInstance().findUserSocket(cmd.arg[0]) != -1)
		throw NicknameInUseException(socket, cmd.arg, user.getNickname());
	user.setNickname(cmd.arg[0]);
}
