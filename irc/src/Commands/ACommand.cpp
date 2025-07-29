#include "ACommand.hpp"

#include "NeedMoreParamsException.hpp"
#include "command.hpp"

ACommand::ACommand(const std::size_t nbArgMin, const std::size_t nbArgMax): nbArgMin_(nbArgMin), nbArgMax_(nbArgMax) {}

void ACommand::checkArgSize(const Command& cmd) const
{
	if (cmd.arg.size() < nbArgMin_ || cmd.arg.size() > nbArgMax_)
		throw NeedMoreParamsException(cmd.name);
}

/**
 * @brief split the character string with a separator passed as a parameter.
 * 
 * @param str  is the string you want to split.
 * @param delimiter separator.
 * 
 * @return a vector of string.
 */
std::vector<std::string> ACommand::splitTargets(const std::string &str) const
{
	const char DELIMITER = ',';

	std::vector<std::string> targetList;
	std::string target;
	std::size_t start = 0;
	std::size_t end = str.find(DELIMITER);

	while (end != std::string::npos)
	{
		target = str.substr(start, end - start);
		targetList.push_back(target);
		start = end + 1;
		end = str.find(DELIMITER, start);
	}
	target = str.substr(start);
	targetList.push_back(target);
	return targetList;
}