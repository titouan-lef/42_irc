#include "NickCmd.hpp"
#include "UnknowCmdException.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "UserManager.hpp"

/**
 * std::string::npos est egale "Pas trouvé" / "valeur invalide" dans une recherche
 */
void NickCmd::execute(std::vector<std::string> args, std::string trailing, User & user) const
{
    if (!user.getNickname().empty())
        return ;
    if (!trailing.empty() || args.size() != 1)
        throw UnknowCmdException("");
    if (args[0].length() > 9 || args[0].empty())
        throw ErroneUsNicknameException(args[0]);
    unsigned char c = args[0][0];
    if (!isalpha(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
        throw ErroneUsNicknameException(args[0]);
    for (size_t i = 0; i < args[0].length(); ++i)
	{
        c = args[0][i];
        if (!isalnum(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
            throw ErroneUsNicknameException(args[0]);
    }
    if (UserManager::getInstance().findUserSocket(args[0]) != -1)
        throw NicknameInUseException(args[0]);
}
