#pragma once

#include "Tester.hpp"
#include <iostream>
#include <map>
#include "CommandManager.hpp"
#include "ACommand.hpp"
#include "User.hpp"
//for user
#include <netinet/in.h>		// struct sockaddr_in
#include <cstring>			// memset()

#include "UserTester.hpp"	// dummyAddr()

class CommandTester
{
public:
	void testerCommand()
	{
		testcmd();
	};

	void testcmd()
	{
		Tester::startTest("Parser");
		User user(1, dummyAddr(1234));
		MsgServ::Command cmdtester;
		cmdtester.name = "PASS";
		cmdtester.arg = {"Password"};
		MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("PASS                             Password                       ", user);
		Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
		Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
		Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
		Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
	};
};