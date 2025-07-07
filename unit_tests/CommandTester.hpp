#pragma once

#include "Tester.hpp"

#include "CommandManager.hpp"
#include "UserManager.hpp"
#include "Server.hpp"
#include "UnknowCmdException.hpp"
#include "NeedMoreParamsException.hpp"
#include "PasswordMismatchException.hpp"
#include "command.hpp"

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
		const Command cmdtester = {"PASS", "", {"Password"}};
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password");
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("        PASS       Password");
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password         ");
			Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
			Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
		}
		{
			Tester::startTest("Parser Unvalid PASS");
			Server::getInstance().definePassword("Password");
			UserManager::getInstance().addNewUser(1, dummyAddr(1234));
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("PASS	Password  :   ", 1);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("         	            ", 1);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("PASS/Password :", 1);});
			}
			{
				Tester::assertThrows<NeedMoreParamsException>([&]() {CommandManager::getInstance().interpretData("PASS Password :", 1);});
			}
			{
				Tester::assertThrows<PasswordMismatchException>([&]() {CommandManager::getInstance().interpretData("PASS Passsword	", 1);});
			}
			{
				Tester::assertThrows<UnknowCmdException>([&]() {CommandManager::getInstance().interpretData("	PASS Password", 1);});
			}
			UserManager::getInstance().deleteUser(1);
		}
	};
};