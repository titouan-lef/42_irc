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
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertFalse(cmdIrc.hasTrailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertFalse(cmdIrc.hasTrailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("        PASS       Password");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertFalse(cmdIrc.hasTrailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS       Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertFalse(cmdIrc.hasTrailing);
		}
		{
			Command cmdIrc = CommandManager::getInstance().parserCmd("PASS Password         ");
			Tester::assertExpectedEqualsActual(commandName, cmdIrc.name);
			Tester::assertExpectedEqualsActual(nbArg, cmdIrc.arg.size());
			Tester::assertExpectedEqualsActual(argValue, cmdIrc.arg[0]);
			Tester::assertFalse(cmdIrc.hasTrailing);
		}
		{
			Tester::startTest("Parser valid PRIVMSG");
			{
				const Command cmdtester = {"PRIVMSG", {"titouan,lucien,pl", "salut la team"}, true};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertTrue(cmdIrc.hasTrailing);
				Tester::assertExpectedEqualsActual(cmdtester.arg[1], cmdIrc.arg[1]);
			}
			{
				const Command cmdtester = {"PRIVMSG", {"titouan,lucien,pl", "   salut la team"}, true};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :   salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertTrue(cmdIrc.hasTrailing);
				Tester::assertExpectedEqualsActual(cmdtester.arg[1], cmdIrc.arg[1]);
			}
		}
		{
			Tester::startTest("Parser valid PRIVMSG");
			{
				const Command cmdtester = {"PRIVMSG", {"titouan,lucien,pl", "salut la team"}, true};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertTrue(cmdIrc.hasTrailing);
				Tester::assertExpectedEqualsActual(cmdtester.arg[1], cmdIrc.arg[1]);
			}
			{
				const Command cmdtester = {"PRIVMSG", {"titouan,lucien,pl", "   salut la team"}, true};
				Command cmdIrc = CommandManager::getInstance().parserCmd("PRIVMSG titouan,lucien,pl :   salut la team");
				Tester::assertExpectedEqualsActual(cmdtester.name, cmdIrc.name);
				Tester::assertExpectedEqualsActual(cmdtester.arg.size(), cmdIrc.arg.size());
				Tester::assertExpectedEqualsActual(cmdtester.arg[0], cmdIrc.arg[0]);
				Tester::assertTrue(cmdIrc.hasTrailing);
				Tester::assertExpectedEqualsActual(cmdtester.arg[1], cmdIrc.arg[1]);
			}
		}
		{
			Tester::startTest("Parser Unvalid PASS");
			Server::getInstance().definePassword("Password");
			UserManager::getInstance().addNewUser(1, dummyAddr(1234));
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("PASS	Password  :   ", 1, DISCONNECTED);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("         	            ", 1, DISCONNECTED);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("PASS/Password :", 1, DISCONNECTED);});
			}
			{
				Tester::assertThrows<NeedMoreParamsException>([&]() {CommandManager::getInstance().interpretData("PASS Password :", 1, DISCONNECTED);});
			}
			{
				Tester::assertThrows<PasswordMismatchException>([&]() {CommandManager::getInstance().interpretData("PASS Passsword	", 1, DISCONNECTED);});
			}
			{
				Tester::assertThrows<UnknownCmdException>([&]() {CommandManager::getInstance().interpretData("	PASS Password", 1, DISCONNECTED);});
			}
			UserManager::getInstance().deleteUser(1);
		}
	};
};