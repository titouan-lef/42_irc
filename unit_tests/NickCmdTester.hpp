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
#include "NicknameInUseException.hpp"
#include "command.hpp"

class NickCmdTester
{
public:
	void testerNickCmd()
	{
		testPermission();
		testValidNickname();
		testInvalidNickname();
		testNickReply();
	};

	void testPermission()
	{
		Tester::startTest("NICK permission tests");

		NickCmd nick;

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));

		Command cmd = {"NICK", {"me"}, false};
		Tester::assertThrows<PasswordMismatchException>([&]() {nick.execute(cmd ,10);});

		UserManager::getInstance().getUser(10).validConnection();

		cmd = {"NICK", {"me", "blabla"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {nick.execute(cmd ,10);});

		cmd = {"NICK", {"me", "and", "me"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {nick.execute(cmd ,10);});

		cmd = {"NICK", {""}, false};
		Tester::assertThrows<NoNicknameGivenException>([&]() {nick.execute(cmd ,10);});

		UserManager::getInstance().addNewUser(11, dummyAddr(4242));
		UserManager::getInstance().getUser(11).setNickname("me");
		UserManager::getInstance().getUser(11).validConnection();

		cmd = {"NICK", {"me"}, false};
		Tester::assertThrows<NicknameInUseException>([&]() {nick.execute(cmd ,10);});\

		UserManager::getInstance().deleteUser(1);
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
			Command cmd = {"NICK", {validNicks[i]}, false};
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
			Command cmd = {"NICK", {invalidNicks[i]}, false};
			Tester::assertThrows<ErroneUsNicknameException>([&]() {nick.execute(cmd ,10);});
		}

		UserManager::getInstance().deleteUser(1);
	};

	void testNickReply()
	{
		Tester::startTest("NICK reply test");

		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("NICK user2");
			user1.assertReply(":user1!user1@localhost NICK user2");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("NICK user2");
			user1.assertReply(":ircserv 433 user1 user2 :Nickname is already in use");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("NICK user2 error");
			user1.assertReply(":ircserv 461 user1 NICK :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("NICK user3");
			user1.assertReply(":user1!user1@localhost NICK user3");
			user2.assertReply(":user1!user1@localhost NICK user3");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #join");
			user1.sendCommand("JOIN #chan");
			user2.sendCommand("JOIN #join");
			user2.sendCommand("JOIN #chan");
			user2.skipReplies(8);
			user1.skipReplies(10);
			user1.sendCommand("NICK user3");
			user1.assertReply(":user1!user1@localhost NICK user3");
			user2.assertReply(":user1!user1@localhost NICK user3");
		}
	};
};