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
		Tester::startTest("List test parsing");

		ListCmd list;

		Command cmd = {"LIST", ":", {"#salut"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {list.execute(cmd, 1);});
		cmd = {"LIST", "", {"#CHAN1", "#CHAN2"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {list.execute(cmd, 1);});
		cmd = {"LIST", ":", {}};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1);});
		cmd = {"LIST", "", {}};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1);});
		cmd = {"LIST", ":#CHAN1,#CHAN2", {}};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1);});
		cmd = {"LIST", "", {"#CHAN1,#CHAN2"}};
		Tester::assertDoNotThrows([&]() {list.execute(cmd, 1);});
	}
	void testListAllChan()
	{
		Tester::startTest("List all channel");
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			usleep(50000);
			user1.skipReply();
			user1.sendCommand("LIST :");
			usleep(50000);
			user1.assertReply(":ircserv 322 #CHAN1 1\r\n:ircserv 322 #CHAN2 1\r\n:ircserv 322 #CHAN3 1\r\n:ircserv 323 :End of LIST");
			user1.sendCommand("LIST");
			usleep(50000);
			user1.assertReply(":ircserv 322 #CHAN1 1\r\n:ircserv 322 #CHAN2 1\r\n:ircserv 322 #CHAN3 1\r\n:ircserv 323 :End of LIST");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			user1.sendCommand("LIST");
			user1.sendCommand("LIST :");
			user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
			usleep(50000);
			user1.skipReply();
			user1.sendCommand("LIST :");
			usleep(50000);
			user1.assertReply(":ircserv 322 #CHAN1 1\r\n:ircserv 322 #CHAN2 1\r\n:ircserv 322 #CHAN3 1\r\n:ircserv 323 :End of LIST");
			user1.sendCommand("LIST");
			usleep(50000);
			user1.assertReply(":ircserv 322 #CHAN1 1\r\n:ircserv 322 #CHAN2 1\r\n:ircserv 322 #CHAN3 1\r\n:ircserv 323 :End of LIST");
		}
	}
	void testListChan()
	{
		Tester::startTest("List selection channel");

		ReplyTester user1(4343, "password", "user1");

		user1.sendCommand("LIST #CHAN1");
		usleep(50000);
		user1.assertReply(":ircserv 403 #CHAN1 :No such channel\r\n:ircserv 323 :End of LIST");

		user1.sendCommand("JOIN #CHAN1,#CHAN2,#CHAN3");
		usleep(50000);
		user1.skipReply();

		user1.sendCommand("LIST #CHAN1,#CHAN3");
		usleep(50000);
		user1.assertReply(":ircserv 322 #CHAN1 1\r\n:ircserv 322 #CHAN3 1\r\n:ircserv 323 :End of LIST");
	}
	void testListChanWithMultipleUser()
	{
		Tester::startTest("List selection channel with multiple user");

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
		usleep(50000);
		user1.skipReply();

		user1.sendCommand("LIST #CHAN1,#CHAN3");
		usleep(50000);
		user1.assertReply(":ircserv 322 #CHAN1 4\r\n:ircserv 322 #CHAN3 4\r\n:ircserv 323 :End of LIST");

		user1.sendCommand("LIST #CHAN2");
		usleep(50000);
		user1.assertReply(":ircserv 322 #CHAN2 5\r\n:ircserv 323 :End of LIST");
	}
};