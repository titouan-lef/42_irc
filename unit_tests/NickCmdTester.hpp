#pragma once

#include "Tester.hpp"
#include "NickCmd.hpp"
#include "UserTester.hpp"
#include "Server.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "ErroneUsNicknameException.hpp"

class NickCmdTester
{
public:
	void testerNickCmd()
	{
		testPermission();
		testValidNickname();
		testInvalidNickname();
	};

	void testPermission()
	{
		Tester::startTest("NICK permission tests");

		NickCmd nick;

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));

		MsgServ::Command cmd = {"NICK", "", {"me"}};
		Tester::assertThrows<PasswordMismatchException>([&]() {nick.execute(cmd ,10);});

		UserManager::getInstance().getUser(10).validConnection();

		cmd = {"NICK", "blabla", {"me"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {nick.execute(cmd ,10);});

		cmd = {"NICK", "", {"me", "and", "me"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {nick.execute(cmd ,10);});

		cmd = {"NICK", "", {""}};
		Tester::assertThrows<NoNicknameGivenException>([&]() {nick.execute(cmd ,10);});

		UserManager::getInstance().deleteUser(1);
	};

	void testValidNickname()
	{
		Tester::startTest("NICK valid nicknames");

		NickCmd nick;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		User& user = UserManager::getInstance().getUser(10);
		user.validConnection();
		std::vector<std::string> validNicks = {
			"a", "Z_", "a1", "abc123", "xYz", "Bob_", "_user", "x-y", "a[]b", "a^b", "`user", "{nick}",
			"|test", "NickName", "Z9_", "z8-", "t0_", "ABc_123", "_[]\\`^{}", "AbcDefG", "name4U", "U1_", "U9-U9"
		};

		for (std::size_t i = 0; i < validNicks.size(); ++i)
		{
			MsgServ::Command cmd = {"NICK", "", {validNicks[i]}};
			Tester::assertDoNotThrows([&]() {nick.execute(cmd, 10);});
			Tester::assertExpectedEqualsActual(validNicks[i], user.getNickname());
		}

		UserManager::getInstance().deleteUser(1);
	};

	void testInvalidNickname()
	{
		Tester::startTest("NICK invalid nicknames");

		NickCmd nick;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		User& user = UserManager::getInstance().getUser(10);
		user.validConnection();

		std::vector<std::string> invalidNicks = {
			"1abc", "-nick", "@user", "#chan", ",nick", "nick name", "nick!", "nick@", "me\n", "me\r", "me\t", "nick=eq",
			"nick+plus", "nick:colon", "aaaaaaaaaa", "nick$", "user%", "user&", "*nick", "(nick)", "\"nick\"", "\'nick\'"
		};

		for (std::size_t i = 0; i < invalidNicks.size(); ++i)
		{
			MsgServ::Command cmd = {"NICK", "", {invalidNicks[i]}};
			Tester::assertThrows<ErroneUsNicknameException>([&]() {nick.execute(cmd ,10);});
		}

		UserManager::getInstance().deleteUser(1);
	};
};