
#include "PassCmd.hpp"
#include "UnknowCmd.hpp"

void PassCmd::execute(std::vector<std::string> args, std::string trailing, const User & user) const
{
    if (!trailing.empty() || args.size() > 1)
        throw UnknowCmd("");
    
}