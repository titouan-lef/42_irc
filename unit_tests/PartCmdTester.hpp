#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "PartCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"
#include "command.hpp"

class PartCmdTester
{
public:
	void testerPartCmd()
	{
		testPermission();
		testPartChannels();
		testPartReply();/** test with messages */
	};

	void testPermission()
	{
		Tester::startTest("PART permission tests");

		PartCmd part;

		Command cmd = {"PART", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {part.execute(cmd ,10);});

		cmd = {"PART", "", {"#test", "#test2", "#test3"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {part.execute(cmd ,10);});

	};

	void testPartChannels()
	{
		Tester::startTest("PART channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		PartCmd part;

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		Command cmd = {"PART", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		ChanServ::getInstance().findOrCreateChannel("#chan", 10);
		cmd = {"PART", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		ChanServ::getInstance().findOrCreateChannel("#join", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		cmd = {"PART", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));

		ChanServ::getInstance().findOrCreateChannel("#chan", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#chan", 10).connectUser(10);});
		cmd = {"PART", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#chan").isConnected(10));

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 1);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 1);});
		UserManager::getInstance().deleteUser(1);
	};

	void testPartReply()
	{
		Tester::startTest("Part channels Reply");

		{	
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #CHAN1,#CHAN2");
			user2.sendCommand("JOIN #CHAN1");
			user2.skipReplies(4);
			user1.skipReplies(9);

			user2.sendCommand("PART #CHAN1 :salut !");
			user1.assertReply(":user2!user2@localhost PART #CHAN1 :salut !");
			user2.assertReply(":user2!user2@localhost PART #CHAN1 :salut !");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user2.sendCommand("JOIN #CHAN1");
			user2.skipReplies(4);
			user1.sendCommand("PART #CHAN1 :salut !");
			user1.assertReply(":ircserv 441 user1 user1 #CHAN1 :They aren't on that channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");
			ReplyTester user3(4343, "password", "user3");

			user1.sendCommand("JOIN #CHAN1,#CHAN2");
			user2.sendCommand("JOIN #CHAN1");
			user3.sendCommand("JOIN #CHAN2");
			user3.skipReplies(4);
			user2.skipReplies(4);
			user1.skipReplies(10);

			user1.sendCommand("PART #CHAN1,#CHAN2 :salut !");
			user1.assertReply(":user1!user1@localhost PART #CHAN1 :salut !");
			user1.assertReply(":user1!user1@localhost PART #CHAN2 :salut !");
			user2.assertReply(":user1!user1@localhost PART #CHAN1 :salut !");
			user3.assertReply(":user1!user1@localhost PART #CHAN2 :salut !");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #CHAN1");
			user2.sendCommand("JOIN #CHAN2");
			user2.skipReplies(4);
			user1.skipReplies(4);

			user1.sendCommand("PART #CHAN1 :salut !");
			user1.assertReply(":user1!user1@localhost PART #CHAN1 :salut !");
			
			user2.sendCommand("LIST");
			user2.assertReply(":ircserv 322 user2 #CHAN2 1");
			user2.assertReply(":ircserv 323 user2 :End of LIST");
		}
	};
};