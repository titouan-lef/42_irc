#include "NickCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "ChanServ.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "NoNicknameGivenException.hpp"
#include "UserManager.hpp"
#include "User.hpp"

NickCmd::NickCmd(): ACommand(VERIFIED, CONNECTED, 1, 1) {}

void NickCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	const std::string nickname = checkCmd(cmd);
	if (userStatus >= IDENTIFIED)
		ChanServ::getInstance().sendBroacastToLinkedUsers(senderSocket, cmd);
	User& user = UserManager::getInstance().getUser(senderSocket);
	updateUser(user, nickname, userStatus);
}

const std::string NickCmd::checkCmd(const Command& cmd) const
{
	checkArgSize(cmd);
	const std::string& nickname = cmd.arg.at(0);
	checkNickname(nickname);
	return nickname;
}

void NickCmd::checkNickname(const std::string& nickname) const
{
	checkNicknameSize(nickname);
	checkNicknameChar(nickname);
	if (UserManager::getInstance().isNickUsed(nickname))
		throw NicknameInUseException(nickname);
}

void NickCmd::checkNicknameSize(const std::string& nickname) const
{
	const size_t MAX_NICK_LENGTH = 9;

	if (nickname.empty())
		throw NoNicknameGivenException();
	if (nickname.length() > MAX_NICK_LENGTH)
		throw ErroneUsNicknameException(nickname);
}

void NickCmd::checkNicknameChar(const std::string& nickname) const
{
	if (!isAuthorizedFirstChar(nickname[0]))
		throw ErroneUsNicknameException(nickname);
	for (size_t i = 1; i < nickname.length(); ++i)
	{
		if (!isAuthorizedChar(nickname[i]))
			throw ErroneUsNicknameException(nickname);
	}
}

bool NickCmd::isAuthorizedChar(const unsigned char c) const
{
	const unsigned char AUTHORIZED_CHAR = '-';

	return (c == AUTHORIZED_CHAR || isdigit(c) || isAuthorizedFirstChar(c));
}

bool NickCmd::isAuthorizedFirstChar(const unsigned char c) const
{
	const std::string AUTHORIZED_CHAR_LIST = "_[]\\`^{}|";

	return (isalpha(c) || AUTHORIZED_CHAR_LIST.find(c) != std::string::npos);
}

void NickCmd::updateUser(User &user, const std::string &nickname, const Status& userStatus) const
{
	user.setNickname(nickname);
	if (userStatus < IDENTIFIED)
		user.passToNextConnectionStep();
}
