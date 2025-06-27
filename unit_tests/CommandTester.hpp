#pragma once

#include "Tester.hpp"
#include <iostream>
#include <map>
#include "CommandManager.hpp"
#include "ACommand.hpp"
#include "User.hpp"
#include "UnknowCmdException.hpp"
#include "NotRegisteredException.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "PasswordMismatchException.hpp"
#include "Server.hpp"
//for user
#include <netinet/in.h>		// struct sockaddr_in
#include <cstring>			// memset()

#include "UserTester.hpp"	// dummyAddr()

class CommandTester
{
public:
	void testerCommand()
	{
		testPass();
	};

	void testPass()
	{
		
		Tester::startTest("Parser Valid PASS");
		User user(1, dummyAddr(1234));
		const MsgServ::Command cmdtester = {"PASS", "", {"Password"}};
		{
			MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password", user);
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ", user);
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("        PASS       Password", user);
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ", user);
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			MsgServ::Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password         ", user);
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Server::getInstance().definePassword("Password");
			Tester::startTest("Parser Unvalid PASS");
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("PASS	Password  :   ", user);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("         	            ", user);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("PASS/Password :", user);});
			}
			{
				Tester::assertThrows<NeedMoreParamsException>([&]() {CommandManager::getInstance().interpretData("PASS Password :", user);});
			}
			{
				Tester::assertThrows<PasswordMismatchException>([&]() {CommandManager::getInstance().interpretData("PASS Passsword	", user);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("	PASS Password", user);});
			}
		}
	};
};