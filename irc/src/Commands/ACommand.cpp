#include "ACommand.hpp"
#include "NeedMoreParamsException.hpp"

std::vector<std::string> ACommand::split(const std::string& str, const char delimiter) const
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter, start);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}

Command ACommand::truncateTrailing(const Command& cmd) const
{
	Command newCmd;
	newCmd.name = cmd.name;
	newCmd.arg = cmd.arg;
	if (!cmd.trailing.empty())
		newCmd.trailing = cmd.trailing.substr(1);
	return (newCmd);
}

std::string ACommand::getLastParam(const Command& cmd, const size_t nbArgsRequiered) const
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
		return cmd.trailing.substr(1);
	}
}

std::string ACommand::getLastParam(const Command& cmd, const size_t argsMin, const size_t argsMax) const
{
	if (cmd.trailing.empty())
	{
		if (cmd.arg.size() > argsMax || cmd.arg.size() < argsMin)
			throw NeedMoreParamsException(cmd.name);
		return cmd.arg.back();
	}
	else
	{
		if (cmd.arg.size() > argsMax - 1 || cmd.arg.size() < argsMin - 1)
			throw NeedMoreParamsException(cmd.name);
		return cmd.trailing.substr(1);
	}
}

size_t ACommand::getNbArgs(const Command& cmd) const
{
	size_t nbArgs = 0;
	nbArgs = cmd.arg.size();
	if (!cmd.trailing.empty())
		nbArgs++;
	return nbArgs;
}