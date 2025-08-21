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
		Channel& chan = ChanServ::getInstance().createChannelWithOperator(std::string("#test"), 1);
		testPermission(chan);
		testInvite(chan);
		ChanServ::getInstance().partAllChannels(1);
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("INVITE permission tests");

		InviteCmd invite;

		Command cmd = {"INVITE", {}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd,1, CONNECTED);});

		cmd = {"INVITE", {"#test1"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd,1, CONNECTED);});

		cmd = {"INVITE", {"user1", "#test", "error"}, false};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd,1, CONNECTED);});

		cmd = {"INVITE", {"user1", "#test", "error"}, true};
		Tester::assertThrows<NeedMoreParamsException>([&]() {invite.execute(cmd,1, CONNECTED);});

		UserManagerTester::createNewTestUser(10, "user1");
		cmd = {"INVITE", {"user1", "#testre"}, false};
		Tester::assertThrows<NoSuchChannelException>([&]() {invite.execute(cmd,1, CONNECTED);});

		cmd = {"INVITE", {"user1", "#test"}, false};
		Tester::assertThrows<NotOnChannelException>([&]() {invite.execute(cmd,10, CONNECTED);});

		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		
		UserManagerTester::createNewTestUser(11, "user2");
		cmd = {"INVITE", {"user2", "#test"}, false};
		Tester::assertDoNotThrows([&]() {invite.execute(cmd,10, CONNECTED);});
		Tester::assertDoNotThrows([&]() {chan.enableInviteRestriction();});
		Tester::assertThrows<ChannelPrivilegesNeededException>([&]() {invite.execute(cmd,10, CONNECTED);});
		UserManager::getInstance().deleteUser(2);

		cmd = {"INVITE", {"user1", "#test"}, false};
		Tester::assertThrows<UserOnChannelException>([&]() {invite.execute(cmd,1, CONNECTED);});

		UserManager::getInstance().deleteUser(1);
		ChanServ::getInstance().partAllChannels(10);
	};

	void testInvite(Channel& chan)
	{
		Tester::startTest("INVITE managment tests");
		{
			InviteCmd invite;

			chan.enableInviteRestriction();
			UserManagerTester::createNewTestUser(10, "user1");
			Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(10);});

			Command cmd = {"INVITE", {"user1", "#test"}, false};
			Tester::assertDoNotThrows([&]() {invite.execute(cmd, 1, CONNECTED);});
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
			user1.skipReplies(5);

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 473 user2 #test :You're not invited to join this channel");

			user1.sendCommand("INVITE user2 #test");
			user2.assertReply(":user1!user1@localhost INVITE user2 #test");
			user2.sendCommand("JOIN #test");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
			user1.skipReplies(1);

			user1.sendCommand("INVITE user2 #test");
			user1.assertReply(":ircserv 443 user1 user2 #test :is already on channel");
		}
	};
};