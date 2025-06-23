#include "PassCmd.hpp"
#include "UnknowCmd.hpp"
#include "Server.hpp"

void PassCmd::execute(std::vector<std::string> args, std::string trailing, const User & user) const
{
	if (!trailing.empty() || args.size() != 1 || args[0].empty())
		throw UnknowCmd("");
	(void)user;
	/**
	 @todo 
	 if (args[0] != globalServerPtr) 
		throw UnknowCmd("");*/

	
}