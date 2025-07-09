#include "NickCmd.hpp"

#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"

#define MAX_NICK_LENGTH 9
#define MAX_ARGS 1

void NickCmd::execute(const Command& cmd, const int socket) const
{
	User &user = UserManager::getInstance().getUser(socket);
	if (!user.isConnected())
		throw PasswordMismatchException();
	std::string Nickname;
	if (cmd.trailing.empty())
	{
		if (cmd.arg.size() != MAX_ARGS)
			throw NeedMoreParamsException(cmd.name);
		Nickname = cmd.arg.at(0);
	}
	else
	{
		if (cmd.arg.size() != 0)
			throw NeedMoreParamsException(cmd.name);
		Nickname = cmd.trailing;
	}
	NickCmd::parsingNickname(Nickname);
	if (UserManager::getInstance().findUserSocket(Nickname) != -1)
		throw NicknameInUseException(Nickname);
	user.setNickname(Nickname);
}

void NickCmd::parsingNickname(const std::string& Nickname) const
{
	if (Nickname.empty())
		throw NoNicknameGivenException();
	if (Nickname.length() > MAX_NICK_LENGTH)
		throw ErroneUsNicknameException(Nickname);
	std::string specials = "_[]\\`^{}|";
	unsigned char c = Nickname[0];
	if ((!isalpha(c)) && specials.find(c) == std::string::npos)
		throw ErroneUsNicknameException(Nickname);
	for (size_t i = 1; i < Nickname.length(); ++i)
	{
		c = Nickname[i];
		if ((!isalnum(c)) && specials.find(c) == std::string::npos && c != '-')
			throw ErroneUsNicknameException(Nickname);
	}
}
