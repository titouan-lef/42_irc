#pragma once

#include "ACommand.hpp"

#include <iostream>
#include <map>

#include "command.hpp"

enum Status;

class CommandTester; /** @todo remove before push */
class PrivMsgTester; /** @todo remove before push */

class CommandManager
{
public:
	static CommandManager& getInstance();
	void interpretData(const std::string& recv, const int senderSocket, const Status& userStatus);

private:
	CommandManager();
	virtual ~CommandManager();

	Command parserCmd(const std::string& recv) const;
	std::size_t createCmdName(const std::string& recv, std::string& cmdName) const;
	bool cmdDoNotExist(const std::string& cmdName) const;
	void parserCmdElement(const std::string &recv, std::size_t start, Command& cmd) const;
	std::size_t delimitParameter(const std::string &recv, std::size_t start, std::string& param) const;

	std::map<std::string, ACommand*> commands_;

	friend class CommandTester ;	/** @todo remove before push */
	friend class PrivMsgTester ;	/** @todo remove before push */
};