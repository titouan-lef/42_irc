#pragma once

#include "Tester.hpp"
#include "ChanServ.hpp"
#include "TopicCmd.hpp"
#include "UserManager.hpp"
#include "UserManagerTester.hpp"
#include "UserNotInChannelException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "command.hpp"

class TopicCmdTester
{
public:
	void testerTopicCmd()
	{
		Channel& chan = ChanServ::getInstance().findOrCreateChannel(std::string("#test"), 1);
		testPermission(chan);
		testTopic(chan);
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("TOPIC permission tests");

		TopicCmd topic;
		(void)chan;

		Command cmd = {"TOPIC", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {topic.execute(cmd ,1);});

		cmd = {"TOPIC", "", {"#test1", "#test2"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {topic.execute(cmd ,1);});

		cmd = {"TOPIC", "", {"#testre"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {topic.execute(cmd ,1);});

		cmd = {"TOPIC", "", {"test"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {topic.execute(cmd ,1);});

		cmd = {"TOPIC", ":New topic", {"#test"}};
		Tester::assertThrows<UserNotInChannelException>([&]() {topic.execute(cmd ,10);});

		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});

		chan.enableTopicRestriction();
		cmd = {"TOPIC", ":New topic", {"#test"}};
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {topic.execute(cmd ,10);});
		chan.disableTopicRestriction();

		ChanServ::getInstance().quitAllChannels(10);
	};

	void testTopic(Channel& chan)
	{
		Tester::startTest("TOPIC managment tests");
		{
			TopicCmd mode;
			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});

			Tester::assertFalse(chan.isOperator(10));
			Tester::assertFalse(chan.isOperator(11));

			Command cmd = {"TOPIC", "", {"#test"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});

			cmd = {"MODE", ":New topic is set", {"#test"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertExpectedEqualsActual(std::string("New topic is set"), chan.topic_);

			cmd = {"MODE", ":", {"#test"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertExpectedEqualsActual(std::string(""), chan.topic_);

			ChanServ::getInstance().quitAllChannels(10);
		}
		Tester::startTest("TOPIC managment reply tests");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("TOPIC");
			user1.assertReply(":ircserv 461 TOPIC :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("TOPIC #test :Changing topic");
			user1.assertReply(":ircserv 403 #test :No such channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReply();

			user2.sendCommand("TOPIC #test :Changing topic");
			user2.assertReply(":ircserv 441 user2 #test :They aren't on that channel");
		}
	};
};