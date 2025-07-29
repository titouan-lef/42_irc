#pragma once

#include "Tester.hpp"
#include "PassCmd.hpp"
#include "UserTester.hpp"
#include "Server.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "AlreadyRegistredException.hpp"
#include "command.hpp"

class PassCmdTester
{
public:
	void testerPassCmd()
	{
		testPermission();
	};

	void testPermission()
	{
		Tester::startTest("PASS command tests");

		PassCmd pass;
		Tester::assertThrows<std::runtime_error>([&]() {Server::getInstance().definePassword("");});
		Tester::assertDoNotThrows([&](){Server::getInstance().definePassword("password");});

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));

		Command cmd = {"PASS", {}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"pass", "pass2"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"pass", "hello"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"pass"}, false};
		Tester::assertThrows<PasswordMismatchException>([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"pass"}, false};
		Tester::assertThrows<PasswordMismatchException>([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"password"}, false};
		Tester::assertDoNotThrows([&]() {pass.execute(cmd ,10);});

		cmd = {"PASS", {"password"}, false};
		Tester::assertThrows<AlreadyRegistredException>([&]() {pass.execute(cmd ,10);});

		UserManager::getInstance().deleteUser(1);
	};
};