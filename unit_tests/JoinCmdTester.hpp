#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "JoinCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

/**test cmd : JOIN 0 */
class JoinCmdTester
{
public:
	void testerJoinCmd()
	{
		testPermission();
		testJoinChannels();
		testJoinPasswords();
		testReply();
	};

	void testPermission()
	{
		Tester::startTest("JOIN permission tests");

		JoinCmd join;

		Command cmd = {"JOIN", {}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", {"#test", "#test2", "#test3"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", {"test"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});

	};

	void testJoinChannels()
	{
		Tester::startTest("JOIN channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		JoinCmd join;

		Command cmd = {"JOIN", {"#join"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 10, "");});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});

		cmd = {"JOIN", {"#join,#chan"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 10, "");});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 10, "");});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		UserManager::getInstance().deleteUser(1);
	};

	void testJoinPasswords()
	{
		Tester::startTest("JOIN channels with passwords");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		ChanServ::getInstance().createChannelWithOperator("#join", 1);
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#join").setPassword("password");});
		
		JoinCmd join;

		Command cmd = {"JOIN", {"#join"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));

		cmd = {"JOIN", {"#join", "password"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 10);});

		cmd = {"JOIN", {"#join,#chan"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		cmd = {"JOIN", {"#join,#chan", "password"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		cmd = {"JOIN", {"#chan,#join", "password"}, false};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		UserManager::getInstance().deleteUser(1);
	};

	void testReply()
	{
		Tester::startTest("JOIN reply");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN");
			user1.assertReply(":ircserv 461 user1 JOIN :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1 #chan2 #chan3");
			user1.assertReply(":ircserv 461 user1 JOIN :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1 #chan2 :#chan3");
			user1.assertReply(":ircserv 461 user1 JOIN :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1 #mdp :trailing");
			user1.assertReply(":ircserv 461 user1 JOIN :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			
			user1.sendCommand("PART #chan1");
			user1.assertReply(":user1!user1@localhost PART #chan1");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1 #mdp");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			
			user1.sendCommand("PART #chan1");
			user1.assertReply(":user1!user1@localhost PART #chan1");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN :#chan1");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			
			user1.sendCommand("PART #chan1");
			user1.assertReply(":user1!user1@localhost PART #chan1");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1,#chan2");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			user1.assertReply(":user1!user1@localhost JOIN :#chan2");
			user1.assertReply(":ircserv 331 user1 #chan2 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan2 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan2 :End of NAMES list");
			
			user1.sendCommand("PART #chan1");
			user1.assertReply(":user1!user1@localhost PART #chan1");
			
			user1.sendCommand("PART #chan2");
			user1.assertReply(":user1!user1@localhost PART #chan2");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			user1.sendCommand("JOIN #chan1");
			user1.assertReply(":ircserv 443 user1 user1 #chan1 :is already on channel");
			user1.sendCommand("JOIN #chan1 mdp");
			user1.assertReply(":ircserv 443 user1 user1 #chan1 :is already on channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #chan1");
			user1.assertReply(":user1!user1@localhost JOIN :#chan1");
			user1.assertReply(":ircserv 331 user1 #chan1 :No topic is set");
			user1.assertReply(":ircserv 353 user1 = #chan1 :@user1");
			user1.assertReply(":ircserv 366 user1 #chan1 :End of NAMES list");
			user1.sendCommand("MODE #chan1 +k mdp");
			user1.skipReplies(1);
			user1.sendCommand("JOIN #chan1");
			user1.assertReply(":ircserv 443 user1 user1 #chan1 :is already on channel");
			user1.sendCommand("JOIN #chan1 mdp");
			user1.assertReply(":ircserv 443 user1 user1 #chan1 :is already on channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN 0 mdp");
			user1.assertReply(":ircserv 461 user1 JOIN :Not enough parameters");
		}
	};
};