
#include "NickCmd.hpp"
#include "UnknowCmdException.hpp"
#include "UserManager.hpp"

/**
 * std::string::npos est egale "Pas trouvé" / "valeur invalide" dans une recherche
 */
void NickCmd::execute(std::vector<std::string> args, std::string trailing, const User & user) const
{
    (void)user;
    if (!trailing.empty() || args.size() != 1)
        throw UnknowCmdException("");
    if (args[0].length() > 9 || args[0].empty())
        throw UnknowCmdException("");
    if (!isalpha(args[0][0]) && std::string("_[]\\`^{}|").find(args[0][0]) == std::string::npos)
        throw UnknowCmdException("");
    for (size_t i = 0; i < args[0].length(); ++i)
	{
        unsigned char c = args[0][i];
        if (!isalnum(c) && std::string("_[]\\`^{}|").find(c) == std::string::npos)
        {
            throw std::runtime_error("Error: Use just ascii value 33 at 126");
        }
    }
    if (UserManager::getInstance().findUserSocket(args[0]) == -1)
        throw UnknowCmdException("");
    //add user nick attribute
    
}

