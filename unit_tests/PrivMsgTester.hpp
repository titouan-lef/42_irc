#pragma once

#include "Tester.hpp"
#include "ReplyTester.hpp"

#include "CommandManager.hpp"
#include "PrivmsgCmd.hpp"
#include "UserManager.hpp"
#include "Server.hpp"
#include "UnknowCmdException.hpp"
#include "NeedMoreParamsException.hpp"
#include "PasswordMismatchException.hpp"
#include "NoSuchNickException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoTextToSendException.hpp"
#include "TooManyTargetsException.hpp"
#include "NoRecipientException.hpp"
#include "command.hpp"

class PrivMsgTester
{
public:
	void testerPrivMsg()
	{
		testPermission();
		testCmdPrivMsg();
	};
private :

	void testPermission()
	{
		Tester::startTest("PRIVMSG permission tests");

		PrivmsgCmd privmsg;

		Command cmd = {"PRIVMSG", ":", {"#CHANEL1,#CHANEL2,#CHANEL3,#CHANEL4,#CHANEL5"}};
		Tester::assertThrows<NoTextToSendException>([&]() {privmsg.execute(cmd ,10);});

		cmd = {"PRIVMSG", ": salut", {"#CHANEL1,#CHANEL2,#CHANEL3,#CHANEL4,#CHANEL5"}};
		Tester::assertThrows<TooManyTargetsException>([&]() {privmsg.execute(cmd ,10);});

		cmd = {"PRIVMSG", ": trailing", {}};
		Tester::assertThrows<NoRecipientException>([&]() {privmsg.execute(cmd ,10);});

		cmd = {"PRIVMSG", ": trailing", {"chan", "chan2"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {privmsg.execute(cmd ,10);});

		cmd = {"PRIVMSG", ": trailing", {"user"}};
		Tester::assertThrows<NoSuchNickException>([&]() {privmsg.execute(cmd ,10);});

		cmd = {"PRIVMSG", ": trailing", {"#chan"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {privmsg.execute(cmd ,10);});

	};

	void testCmdPrivMsg()
	{
		Tester::startTest("PRIVMSG send tests");

		ReplyTester user1(4343, "password", "user1");
		ReplyTester user2(4343, "password", "user2");
		ReplyTester user3(4343, "password", "user3");

		user1.sendCommand("PRIVMSG user2 : Hello world !");
		user2.assertReply(":user1!user1@localhost PRIVMSG user2 : Hello world !");

		user2.sendCommand("PRIVMSG user1 : Hello world !");
		user1.assertReply(":user2!user2@localhost PRIVMSG user1 : Hello world !");

		user2.sendCommand("PRIVMSG user2 : Hello world !");
		user2.assertReply(":user2!user2@localhost PRIVMSG user2 : Hello world !");

		user1.sendCommand("PRIVMSG user1 : Hello world !");
		user1.assertReply(":user1!user1@localhost PRIVMSG user1 : Hello world !");

		user1.sendCommand("JOIN #CHAN1");
		user1.skipReply();
		user3.sendCommand("JOIN #CHAN1");
		user3.skipReply();
		user1.skipReply();
		user2.sendCommand("PRIVMSG #CHAN1 : Hello world !");
		user1.assertReply(":user2!user2@localhost PRIVMSG #CHAN1 : Hello world !");
		user3.assertReply(":user2!user2@localhost PRIVMSG #CHAN1 : Hello world !");

		user2.sendCommand("JOIN #CHAN1");
		user3.skipReply();
		user2.skipReply();
		user1.skipReply();
		user2.sendCommand("PRIVMSG #CHAN1 : Hello world !");
		user1.assertReply(":user2!user2@localhost PRIVMSG #CHAN1 : Hello world !");
		user3.assertReply(":user2!user2@localhost PRIVMSG #CHAN1 : Hello world !");
	};
};