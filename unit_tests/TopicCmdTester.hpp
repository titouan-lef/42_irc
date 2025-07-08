#pragma once

#include "Tester.hpp"
#include "ChanServ.hpp"
#include "TopicCmd.hpp"
#include "UserManager.hpp"
#include "UserManagerTester.hpp"
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

		chan.enableTopicRestriction();
		cmd = {"TOPIC", ":New topic", {"#test"}};
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {topic.execute(cmd ,10);});
		chan.disableTopicRestriction();

	};

	void testTopic(Channel& chan)
	{
		Tester::startTest("TOPIC managment tests");

		TopicCmd mode;
		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});

		Tester::assertFalse(chan.isModerator(10));
		Tester::assertFalse(chan.isModerator(11));

		Command cmd = {"TOPIC", "", {"#test"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});

		cmd = {"MODE", ":New topic is set", {"#test"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertExpectedEqualsActual(std::string("New topic is set"), chan.getTopic());

		cmd = {"MODE", ":", {"#test"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertExpectedEqualsActual(std::string(""), chan.getTopic());

		ChanServ::getInstance().quitAllChannels(10);
	};
};