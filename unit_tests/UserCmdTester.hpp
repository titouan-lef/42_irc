#pragma once

#include "Tester.hpp"
#include "UserCmd.hpp"
#include "UserTester.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NotRegisteredException.hpp"
#include "command.hpp"

class UserCmdTester
{
public:
	void testerUserCmd()
	{
		testPermission();
		testValidUsernames();
		testInvalidUsernames();
	};

	void testPermission()
	{
		Tester::startTest("USER permission tests");

		UserCmd userCmd;

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));

		Command cmd = {"USER", {"0", "*", "RealName"}, true};
		Tester::assertThrows<PasswordMismatchException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		UserManager::getInstance().getUser(10).passToNextConnectionStep();

		cmd = {"USER", {"0", "*", "RealName"}, true};
		Tester::assertThrows<NotRegisteredException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		UserManager::getInstance().getUser(10).setNickname("nickname");
		UserManager::getInstance().getUser(10).passToNextConnectionStep();

		cmd = {"USER", {"0", "*"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		cmd = {"USER", {"0", "RealName"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		cmd = {"USER", {"0","0", "RealName"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		cmd = {"USER", {"username","*","*", "RealName"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		cmd = {"USER", {"username","0","0", "RealName"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		cmd = {"USER", {"username","1","*", "RealName"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});

		UserManager::getInstance().getUser(10).setUsername("username");
		UserManager::getInstance().getUser(10).passToNextConnectionStep();

		cmd = {"USER", {"username","0","*", "RealName"}, true};
		Tester::assertThrows<AlreadyRegistredException>([&]() {userCmd.execute(cmd, 10, UserManager::getInstance().getUser(10).getStatus());});\

		UserManager::getInstance().deleteUser(1);
	};

	void testValidUsernames()
	{
		Tester::startTest("USER valid usernames");

		UserCmd userCmd;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		User& user = UserManager::getInstance().getUser(10);
		std::vector<std::string> validUsernames =
		{
			"a", "Z", "a1", "abc123", "xYz", "Bob123", "User1", "User1234", "TestUser", "Nick42", "Alpha9", "Z9", "U0U0", "JohnDoe", "x9y8z7",
			"abcDEF123", "X1X2X3X4", "Valid123", "A1B2C3", "Z999999999"
		};
		for (std::size_t i = 0; i < validUsernames.size(); ++i)
		{
			Command cmd = {"USER", {validUsernames.at(i), "0", "*", "RealName"}, true};
			Tester::assertDoNotThrows([&]() {userCmd.execute(cmd, 10, IDENTIFIED);});
			Tester::assertExpectedEqualsActual(validUsernames.at(i), user.getUsername());
			user.setUsername("");
		}

		UserManager::getInstance().deleteUser(1);
	};

	void testInvalidUsernames()
	{
		Tester::startTest("USER invalid usernames");

		UserCmd userCmd;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		std::vector<std::string> invalidUsernames =
		{
			"user name", "user@name", "hello!", "name*", "line\nbreak", "carriage\r", "tab\tchar", "user\n", "user#",
			"comma,name", "user.name", "semi;colon", "quote\"", "tick'", "slash/", "back\\slash", "left[bracket", "right]bracket",
			"brace{open", "brace}close", "pipe|bar", "caret^", "tilde~", "equals=", "plus+", "dash-", "underscore_", "user$", "percent%",
			"exclaim!", "user😀", "中文", "русский", "123!", "!", " ", "........", "user\tname"
		};

		for (std::size_t i = 0; i < invalidUsernames.size(); ++i)
		{
			Command cmd = {"USER", {invalidUsernames.at(i), "0", "*", "RealName"}, true};
			Tester::assertThrows<NeedMoreParamsException>([&]() {userCmd.execute(cmd, 10, IDENTIFIED);});
		}

		UserManager::getInstance().deleteUser(1);
	};
};