#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "QuitCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NoSuchNickException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"
#include "command.hpp"

class QuitCmdTester
{
public:
	void testerQuitCmd()
	{
		testQuit();
		testQuitReply();
	};


	void testQuit()
	{
		Tester::startTest("QUIT test");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		QuitCmd quit;
		ChanServ::getInstance().createChannelWithOperator("#join", 10);
		Command cmd = {"QUIT", {"Bye"}, true};
		Tester::assertDoNotThrows([&]() {quit.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		Tester::assertThrows<NoSuchNickException>([&]() {UserManager::getInstance().getUser(10);});
	};

	void testQuitReply()
	{
		Tester::startTest("QUIT reply test");

		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");
	
			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("QUIT");
			user2.sendCommand("PRIVMSG #test :Hello everyone");
			user2.assertReply(":ircserv 403 user2 #test :No such channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");
	
			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("QUIT");
			user2.assertReply(":user1!user1@localhost QUIT");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("QUIT :Bye");
			user2.assertReply(":user1!user1@localhost QUIT :Bye");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");
			ReplyTester user3(4343, "password", "user3");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user3.sendCommand("JOIN #test");
			user3.skipReplies(4);
			user2.skipReplies(5);
			user1.skipReplies(6);
			user1.sendCommand("QUIT :Bye bye");
			user2.assertReply(":user1!user1@localhost QUIT :Bye bye");
			user3.assertReply(":user1!user1@localhost QUIT :Bye bye");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #chan");
			user1.sendCommand("JOIN #chan");
			user2.skipReplies(9);
			user1.skipReplies(9);
			user1.sendCommand("QUIT :Bye bye");
			user2.assertReply(":user1!user1@localhost QUIT :Bye bye");
		}
	}
};