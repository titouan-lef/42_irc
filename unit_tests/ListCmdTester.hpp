#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "ListCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

class ListCmdTester
{
public:
	void testerListCmd()
	{
		testParsing();
		testListAllChan();
		testListChan();
		testListChanWithMultipleUser();
	};

	void testParsing()
	{
		Tester::startTest("LIST test parsing");

		ListCmd list;

		Command cmd = {"LIST", {"#salut", ""}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {list.execute(cmd, 1, CONNECTED);});
		cmd = {"LIST", {"#CHAN1", "#CHAN2"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {list.execute(cmd, 1, CONNECTED);});
		cmd = {"LIST", {":"}, true};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1, CONNECTED);});
		cmd = {"LIST", {}, false};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1, CONNECTED);});
		cmd = {"LIST", {"#CHAN1,#CHAN2"}, true};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1, CONNECTED);});
		cmd = {"LIST", {"#CHAN1,#CHAN2"}, false};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1, CONNECTED);});
	}
	void testListAllChan()
	{
		Tester::startTest("LIST all channel");
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			user1.skipReplies(12);
			user1.sendCommand("LIST :");
			user1.assertReply(":ircserv 403 user1 :No such channel");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
			user1.sendCommand("LIST");
			user1.assertReply(":ircserv 322 user1 #CHAN1 1");
			user1.assertReply(":ircserv 322 user1 #CHAN2 1");
			user1.assertReply(":ircserv 322 user1 #CHAN3 1");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("LIST");
			user1.sendCommand("LIST :");
			user1.assertReply(":ircserv 403 user1 :No such channel");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			user1.skipReplies(12);
			user1.sendCommand("LIST :");
			user1.assertReply(":ircserv 403 user1 :No such channel");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
			user1.sendCommand("LIST");
			user1.assertReply(":ircserv 322 user1 #CHAN1 1");
			user1.assertReply(":ircserv 322 user1 #CHAN2 1");
			user1.assertReply(":ircserv 322 user1 #CHAN3 1");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			user1.skipReplies(12);
			user1.sendCommand("LIST :#CHAN1,#CHAN2,#CHAN3");
			user1.assertReply(":ircserv 322 user1 #CHAN1 1");
			user1.assertReply(":ircserv 322 user1 #CHAN2 1");
			user1.assertReply(":ircserv 322 user1 #CHAN3 1");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			user1.skipReplies(12);
			user1.sendCommand("LIST :#CHAN1,#CHAN_NOT_EXIST,#CHAN3");
			user1.assertReply(":ircserv 322 user1 #CHAN1 1");
			user1.assertReply(":ircserv 403 user1 #CHAN_NOT_EXIST :No such channel");
			user1.assertReply(":ircserv 322 user1 #CHAN3 1");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			user1.skipReplies(12);
			user1.sendCommand("LIST :#CHAN1 #CHAN2,#CHAN3");
			user1.assertReply(":ircserv 403 user1 #CHAN1 #CHAN2 :No such channel");
			user1.assertReply(":ircserv 322 user1 #CHAN3 1");
			user1.assertReply(":ircserv 323 user1 :End of LIST");
		}
	}
	void testListChan()
	{
		Tester::startTest("LIST selection channel");

		ReplyTester user1(4343, "password", "user1");

		user1.sendCommand("LIST #CHAN1");
		user1.assertReply(":ircserv 403 user1 #CHAN1 :No such channel");
		user1.assertReply(":ircserv 323 user1 :End of LIST");

		user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
		user1.skipReplies(12);

		user1.sendCommand("LIST #CHAN1,#CHAN3");
		user1.assertReply(":ircserv 322 user1 #CHAN1 1");
		user1.assertReply(":ircserv 322 user1 #CHAN3 1");
		user1.assertReply(":ircserv 323 user1 :End of LIST");
	}
	void testListChanWithMultipleUser()
	{
		Tester::startTest("LIST selection channel with multiple user");

		ReplyTester user1(4343, "password", "user1");
		ReplyTester user2(4343, "password", "user2");
		ReplyTester user3(4343, "password", "user3");
		ReplyTester user4(4343, "password", "user4");
		ReplyTester user5(4343, "password", "user5");
		ReplyTester user6(4343, "password", "user6");

		user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
		user2.sendCommand("JOIN #CHAN1,#CHAN2");
		user3.sendCommand("JOIN #CHAN3,#CHAN2");
		user4.sendCommand("JOIN #CHAN1,#CHAN3");
		user5.sendCommand("JOIN #CHAN3,#CHAN2");
		user6.sendCommand("JOIN #CHAN1,#CHAN2");
		user1.skipReplies(22);

		user1.sendCommand("LIST #CHAN1,#CHAN3");
		user1.assertReply(":ircserv 322 user1 #CHAN1 4");
		user1.assertReply(":ircserv 322 user1 #CHAN3 4");
		user1.assertReply(":ircserv 323 user1 :End of LIST");

		user1.sendCommand("LIST #CHAN2");
		user1.assertReply(":ircserv 322 user1 #CHAN2 5");
		user1.assertReply(":ircserv 323 user1 :End of LIST");
	}
};