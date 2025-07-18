#pragma once

#include "Tester.hpp"

#include "CommandManager.hpp"
#include "UserManager.hpp"
#include "Server.hpp"
#include "UnknownCmdException.hpp"
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
		const std::string commandName = "PASS";
		const std::size_t nbArg = 1;
		const std::string argValue = "Password";
		const std::string trailing = "";
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("        PASS       Password");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(trailing, cmdIrc.trailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertExpectedEqualsActual(trailing, cmdIrc.trailing);
		}
		{
			Tester::startTest("Parser valid PRIVMSG");
			{
				const Command cmdtester = {"PRIVMSG", ":salut la team", {"titouan,lucien,pl"}};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
			}
			{
				const Command cmdtester = {"PRIVMSG", ":   salut la team", {"titouan,lucien,pl"}};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :   salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
			}
		}
		{
			Tester::startTest("Parser valid PRIVMSG");
			{
				const Command cmdtester = {"PRIVMSG", ":salut la team", {"titouan,lucien,pl"}};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
			}
			{
				const Command cmdtester = {"PRIVMSG", ":   salut la team", {"titouan,lucien,pl"}};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :   salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertExpectedEqualsActual(cmdtester.trailing, cmdIrc.trailing);
			}
		}
		{
			Tester::startTest("Parser Unvalid PASS");
			Server::getInstance().definePassword("Password");
			UserManager::getInstance().addNewUser(1, dummyAddr(1234));
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("PASS	Password  :   ", 1);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("         	            ", 1);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("PASS/Password :", 1);});
			}
			{
				Tester::assertThrows<NeedMoreParamsException>([&]() {CommandManager::getInstance().interpretData("PASS Password :", 1);});
			}
			{
				Tester::assertThrows<PasswordMismatchException>([&]() {CommandManager::getInstance().interpretData("PASS Passsword	", 1);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("	PASS Password", 1);});
			}
			UserManager::getInstance().deleteUser(1);
		}
	};
};