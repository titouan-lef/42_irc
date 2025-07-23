#include "ACommand.hpp"

#include "command.hpp"
#include "NeedMoreParamsException.hpp"

/**
 * @brief split the character string with a separator passed as a parameter.
 * 
 * @param str  is the string you want to split.
 * @param delimiter separator.
 * 
 * @return a vector of string.
 */
std::vector<std::string> ACommand::splitTargets(const std::string& str) const
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

/**
 * @brief remove ':' of the trailing std::string for the msg or param
 * @return a new ACommand
 */
const std::string ACommand::truncateTrailing(const std::string& trailing) const
{
	if (trailing.empty())
		return trailing;
	return trailing.substr(1);
}

/**
 * @brief Retrieves the last argument from the command, depending on whether a trailing parameter is present.
 * Throws an exception if the required number of arguments is not satisfied.
 * 
 * @param cmd is a actual command
 * @param nbArgsRequierd is a number of argument for the command need
 * @return Last param of the command
 * @throws NeedMoraParamsException the number requierement of args is not the same
 */
std::string ACommand::getLastParam(const Command& cmd, const std::size_t nbArgsRequiered) const
{
	if (cmd.trailing.empty())
	{
		if (cmd.arg.size() != nbArgsRequiered)
			throw NeedMoreParamsException(cmd.name);
		return cmd.arg.back();
	}
	else
	{
		if (cmd.arg.size() != nbArgsRequiered - 1)
			throw NeedMoreParamsException(cmd.name);
		return truncateTrailing(cmd.trailing);
	}
}

/**
 * @brief Retrieves the last argument from the command, depending on whether a trailing parameter is present.
 * Throws an exception if the required number of arguments is not satisfied.
 * 
 * @param cmd is a actual command.
 * @param argsMin is a number min of argument for the command need.
 * @param argsMin is a number max of argument for the command need.
 * 
 * @return Last param of the command
 * @throws NeedMoraParamsException the number requierement of args is not the same
 */
std::string ACommand::getLastParam(const Command& cmd, const std::size_t argsMin, const std::size_t argsMax) const
{
	if (cmd.trailing.empty())
	{
		if (cmd.arg.size() > argsMax || cmd.arg.size() < argsMin)
			throw NeedMoreParamsException(cmd.name);
		return cmd.arg.back();
	}
	else
	{
		if (cmd.arg.size() + 1 > argsMax || cmd.arg.size() + 1 < argsMin)
			throw NeedMoreParamsException(cmd.name);
		return truncateTrailing(cmd.trailing);
	}
}

/**
 * @brief count the number of argument in the command
 * 
 * @param cmd is a actual command.
 * 
 * @return the number of argument
 */
std::size_t ACommand::getNbArg(const Command& cmd) const
{
	const std::size_t nbArg = cmd.arg.size();
	if (cmd.trailing.empty() || cmd.trailing == ":")
		return nbArg;
	return nbArg + 1;
}