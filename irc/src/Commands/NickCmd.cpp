#include "NickCmd.hpp"
#include "ChanServ.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"

#define MAX_NICK_LENGTH 9
#define ARG_REQUIERED 1

void NickCmd::execute(const Command& cmd, const int socket) const
{
	User &user = UserManager::getInstance().getUser(socket);
	if (!user.isConnected())
		throw PasswordMismatchException();
	std::string nickname = getLastParam(cmd, ARG_REQUIERED);
	NickCmd::parsingNickname(nickname);
	if (UserManager::getInstance().isNickUsed(nickname))
		throw NicknameInUseException(nickname);
	if (user.isNickRegister())
		ChanServ::getInstance().sendBroacastToLinkedUsers(socket, cmd.name, cmd.arg, truncateTrailing(cmd.trailing));
	user.setNickname(nickname);
}

void NickCmd::parsingNickname(const std::string& nickname) const
{
	if (nickname.empty())
		throw NoNicknameGivenException();
	if (nickname.length() > MAX_NICK_LENGTH)
		throw ErroneUsNicknameException(nickname);
	std::string specials = "_[]\\`^{}|";
	unsigned char c = nickname[0];
	if ((!isalpha(c)) && specials.find(c) == std::string::npos)
		throw ErroneUsNicknameException(nickname);
	for (size_t i = 1; i < nickname.length(); ++i)
	{
		c = nickname[i];
		if ((!isalnum(c)) && specials.find(c) == std::string::npos && c != '-')
			throw ErroneUsNicknameException(nickname);
	}
}
