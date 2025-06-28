#pragma once

#include <iostream>
#include <map>
#include "ACommand.hpp"
#include "User.hpp"

class CommandTester; /** @todo remove before push */

class CommandManager
{
public:
	static CommandManager& getInstance();
	void interpretData(const std::string& recv, const int socket);
	void registerClient(const std::string& recv, const int socket);

private:
	CommandManager();
	virtual ~CommandManager();
	CommandManager(const CommandManager& other);
	CommandManager& operator=(const CommandManager& other);

	MsgServ::Command parserCmd(const std::string& recv, const int socket);

	std::map<std::string, ACommand*> commands_;

	friend class CommandTester ;	/** @todo remove before push */
};