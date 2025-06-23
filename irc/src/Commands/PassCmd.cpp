#include "PassCmd.hpp"
#include "PasswordMismatchException.hpp"
#include "Server.hpp"

void PassCmd::execute(std::vector<std::string> args, std::string trailing, User & user) const
{
    if (!trailing.empty() || args.size() != 1 || args[0].empty())
        throw PasswordMismatchException();
    if (args[0] != Server::getInstance().getPassword()) 
        throw PasswordMismatchException();
    user.validConnection();
}