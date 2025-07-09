#pragma once

#include "Tester.hpp"

#include "Channel.hpp"
#include "BadChanMaskException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InviteOnlyException.hpp"
#include "UserOnChannelException.hpp"

#include "UserTester.hpp"	// dummyAddr()

class ChannelTester
{
public:
	void testerChannel()
	{
		testInstance();
		testConnection();
		testInvite();
		testPassword();
		testTopic();
		testUserLimit();
	};
 
	void testInstance()
	{
		Tester::startTest("Channel creation");

		Channel chan("mychannel", 10);

		Tester::assertExpectedEqualsActual(std::string("mychannel"), chan.getName());
		Tester::assertFalse(chan.isInviteRestricted());
		Tester::assertFalse(chan.isTopicRestricted());
		Tester::assertTrue(chan.isConnected(10));
		Tester::assertTrue(chan.isOperator(10));
	};

	void testConnection()
	{
		Tester::startTest("Channel connection");

		Channel chan("mychannel", 10);

		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});
		Tester::assertTrue(chan.isConnected(11));
	};

	void testInvite()
	{
		Tester::startTest("Channel invitation");

		Channel chan("mychannel", 10);

		chan.enableInviteRestriction();
		Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(11);});
		Tester::assertFalse(chan.isConnected(11));
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {chan.inviteUser(11, 12);});
		Tester::assertDoNotThrows([&]() {chan.inviteUser(11, 10);});
		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});
		Tester::assertTrue(chan.isConnected(11));
		Tester::assertThrows<UserOnChannelException>([&]() {chan.inviteUser(11, 10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(11);});
		Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(11);});

	};

	void testPassword()
	{
		Tester::startTest("Channel password protection");

		Channel chan("secure", 10);

		Tester::assertDoNotThrows([&]() {chan.connectUser(9, "wrongpass");});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(9);});

		chan.setPassword("secret");

		Tester::assertThrows<BadChanMaskException>([&]() {chan.connectUser(9);});
		Tester::assertThrows<BadChanMaskException>([&]() {chan.connectUser(9, "wrongpass");});
		Tester::assertDoNotThrows([&]() {chan.connectUser(9, "secret");});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(9);});

		chan.disablePassword();

		Tester::assertDoNotThrows([&]() {chan.connectUser(9, "secret");});
	};

	void testTopic()
	{
		Tester::startTest("Channel Topic restriction");

		Channel chan("secure", 10);

		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});
		Tester::assertDoNotThrows([&]() {chan.setTopic("This is a test!", 11);});
		Tester::assertExpectedEqualsActual(std::string("This is a test!"), chan.topic_);
		chan.enableTopicRestriction();
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {chan.setTopic("Changing topic error", 11);});
		Tester::assertDoNotThrows([&]() {chan.setTopic("Changing topic good", 10);});
		Tester::assertExpectedEqualsActual(std::string("Changing topic good"), chan.topic_);
		chan.disableTopicRestriction();
		Tester::assertDoNotThrows([&]() {chan.setTopic("This is a test2!", 11);});
		Tester::assertExpectedEqualsActual(std::string("This is a test2!"), chan.topic_);
	};

	void testUserLimit()
	{
		Tester::startTest("Channel User limit");

		Channel chan("limited", 10);
		chan.setLimitNbUsers(2);

		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});
		Tester::assertThrows<ChannelIsFullException>([&]() {chan.connectUser(12);});
	}
};