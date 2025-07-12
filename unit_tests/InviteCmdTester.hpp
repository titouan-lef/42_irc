#pragma once

#include "Tester.hpp"
#include "ReplyTester.hpp"
#include "ChanServ.hpp"
#include "InviteCmd.hpp"
#include "UserManager.hpp"
#include "UserManagerTester.hpp"
#include "command.hpp"

class InviteCmdTester
{
public:
	void testerInviteCmd()
	{
		Channel& chan = ChanServ::getInstance().findOrCreateChannel(std::string("#test"), 1);
		testPermission(chan);
		testInvite(chan);
		ChanServ::getInstance().partAllChannels(1);
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("INVITE permission tests");

		InviteCmd invite;

		Command cmd = {"INVITE", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd ,1);});

		cmd = {"INVITE", "", {"#test1"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd ,1);});

		cmd = {"INVITE", "", {"user1", "#test", "error"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd ,1);});

		cmd = {"INVITE", "error", {"user1", "#test"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd ,1);});

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		UserManager::getInstance().getUser(10).setNickname("user1");
		cmd = {"INVITE", "", {"user1", "#testre"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {invite.execute(cmd ,1);});

		cmd = {"INVITE", "", {"user1", "#test"}};
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {invite.execute(cmd ,10);});

		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		cmd = {"INVITE", "", {"user1", "#test"}};
		Tester::assertThrows<UserOnChannelException>([&]() {invite.execute(cmd ,1);});

		UserManager::getInstance().deleteUser(1);
		ChanServ::getInstance().partAllChannels(10);
	};

	void testInvite(Channel& chan)
	{
		Tester::startTest("INVITE managment tests");
		{
			InviteCmd invite;

			chan.enableInviteRestriction();
			UserManager::getInstance().addNewUser(10, dummyAddr(4242));
			UserManager::getInstance().getUser(10).setNickname("user1");
			Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(10);});

			Command cmd = {"INVITE", "", {"user1", "#test"}};
			Tester::assertDoNotThrows([&]() {invite.execute(cmd ,1);});
			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});

			UserManager::getInstance().deleteUser(1);
			ChanServ::getInstance().partAllChannels(10);
		}
		Tester::startTest("INVITE managment reply tests");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("INVITE");
			user1.assertReply(":ircserv 461 user1 INVITE :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("INVITE user2 #test");
			user1.assertReply(":ircserv 401 user1 user2 :No such nick/channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("INVITE user1 #test");
			user1.assertReply(":ircserv 403 user1 #test :No such channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.sendCommand("MODE #test +i");
			user1.skipReply();

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 473 user2 #test :You're not invited to join this channel");

			user1.sendCommand("INVITE user2 #test");
			usleep(50000);
			user2.sendCommand("JOIN #test");
			user2.assertReply(":user2!user2@localhost JOIN :#test\r\n:ircserv 331 user2 #test :No topic is set\r\n:ircserv 353 user2 = #test :@user1 user2\r\n:ircserv 366 user2 #test :End of NAMES list");
			user1.skipReply();

			user1.sendCommand("INVITE user2 #test");
			user1.assertReply(":ircserv 443 user1 user2 #test :is already on channel");
		}
	};
};