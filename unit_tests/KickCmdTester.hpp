#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "KickCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

class KickCmdTester
{
public:
	void testerKickCmd()
	{
		testPermission();
		testKickChannels();
		testKickReply();
	};

	void testPermission()
	{
		Tester::startTest("KICK permission tests");

		KickCmd kick;

		Command cmd = {"KICK", {}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", {"#test"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", {"#test", "#test2", "#test3", "reason"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", {"#test,#chan", "user"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", {"#test,#chan", "user,user1,user2"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});
	};

	void testKickChannels()
	{
		Tester::startTest("KICK users from channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		UserManager::getInstance().getUser(10).setNickname("Bob");
		KickCmd kick;
		Command cmd;
		{
			ChanServ::getInstance().createChannelWithOperator("#join", 10);
			cmd = {"KICK", {"#join", "Bob"}, false};
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
			Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		}
		{
			ChanServ::getInstance().createChannelWithOperator("#join", 10);
			ChanServ::getInstance().createChannelWithOperator("#chan", 10);
			cmd = {"KICK", {"#join,#chan", "Bob,Bob"}, false};
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
			Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
			Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});
		}
		ChanServ::getInstance().createChannelWithOperator("#join", 1);
		{
			Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#join").connectUser(10);});
			cmd = {"KICK", {"#join", "Bob"}, false};
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
			Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
			Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));
		}
		ChanServ::getInstance().createChannelWithOperator("#chan", 1);
		{
			Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#join").connectUser(10);});
			Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#chan").connectUser(10);});
			cmd = {"KICK", {"#join,#chan", "Bob,Bob"}, false};
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
			Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
			Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
			Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));
			Tester::assertFalse(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		}
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#join", 1);});
		{
			Tester::assertDoNotThrows([&]() {ChanServ::getInstance().createChannelWithOperator("#join", 10);});
			Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#chan").connectUser(10);});
			cmd = {"KICK", {"#join,#chan", "Bob,Bob"}, false};
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
			Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
			Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
			Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
			Tester::assertFalse(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		}
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().part("#chan", 1);});
		UserManager::getInstance().deleteUser(1);
	};

		void testKickReply()
	{
		Tester::startTest("KICK replies test");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #join");
			user1.skipReplies(4);
			user1.sendCommand("KICK #join user1");
			user1.assertReply(":user1!user1@localhost KICK #join user1 :Kicked by user1");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #join");
			user1.skipReplies(4);
			user1.sendCommand("KICK #join user1 :You cannot talk like that!");
			user1.assertReply(":user1!user1@localhost KICK #join user1 :You cannot talk like that!");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #join");
			user1.sendCommand("JOIN #chan");
			user1.skipReplies(8);
			user1.sendCommand("KICK #join,#chan user1,user1");
			user1.assertReply(":user1!user1@localhost KICK #join user1 :Kicked by user1");
			user1.assertReply(":user1!user1@localhost KICK #chan user1 :Kicked by user1");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #join");
			user2.sendCommand("JOIN #join");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user2.sendCommand("KICK #join user1");
			user2.assertReply(":ircserv 482 user2 #join :You're not channel operator");
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
			user2.sendCommand("KICK #join,#chan user1,user1");
			user2.assertReply(":ircserv 482 user2 #join :You're not channel operator");
			user2.assertReply(":ircserv 482 user2 #chan :You're not channel operator");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #join");
			user2.sendCommand("JOIN #chan");
			user2.sendCommand("JOIN #join");
			user1.sendCommand("JOIN #chan");
			user2.skipReplies(9);
			user1.skipReplies(9);

			user2.sendCommand("KICK #join,#chan user1,user1");
			user2.assertReply(":ircserv 482 user2 #join :You're not channel operator");
			user2.assertReply(":user2!user2@localhost KICK #chan user1 :Kicked by user2");
			user1.assertReply(":user2!user2@localhost KICK #chan user1 :Kicked by user2");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");
			ReplyTester user3(4343, "password", "user3");

			user1.sendCommand("JOIN #join");
			user2.sendCommand("JOIN #join");
			user3.sendCommand("JOIN #join");
			user3.skipReplies(4);
			user2.skipReplies(5);
			user1.skipReplies(6);
			user1.sendCommand("KICK #join user2,user3");
			user1.assertReply(":user1!user1@localhost KICK #join user2 :Kicked by user1");
			user1.assertReply(":user1!user1@localhost KICK #join user3 :Kicked by user1");
			user2.assertReply(":user1!user1@localhost KICK #join user2 :Kicked by user1");
			user3.assertReply(":user1!user1@localhost KICK #join user2 :Kicked by user1");
			user3.assertReply(":user1!user1@localhost KICK #join user3 :Kicked by user1");
		}
	};
};