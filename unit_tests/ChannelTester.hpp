#pragma once

#include "Tester.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "NoSuchNickException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InviteOnlyException.hpp"
#include "InviteOnlyException.hpp"

#include <netinet/in.h>		// struct sockaddr_in
#include <cstring>			// memset()

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

		User moderator(1, dummyAddr(1234));
		moderator.setNickname("Alice");
		Channel chan("mychannel", moderator);

		Tester::assertExpectedEqualsActual(std::string("mychannel"), chan.getName());
		Tester::assertFalse(chan.isInviteRestricted());
		Tester::assertFalse(chan.isTopicRestricted());
		Tester::assertTrue(chan.isConnected("Alice"));
		Tester::assertTrue(chan.isModerator("Alice"));

	};

	void testConnection()
	{
		Tester::startTest("Channel connection");

		User moderator(1, dummyAddr(1234));
		moderator.setNickname("Alice");
		User user(1, dummyAddr(1234));
		user.setNickname("Rabbit");
		Channel chan("mychannel", moderator);

		Tester::assertDoNotThrows([&]() {chan.connectUser(user);});
		Tester::assertTrue(chan.isConnected("Rabbit"));

	};

	void testInvite()
	{
		Tester::startTest("Channel invitation");

		User moderator(1, dummyAddr(1234));
		moderator.setNickname("Alice");
		User user(1, dummyAddr(1234));
		user.setNickname("Rabbit");
		Channel chan("mychannel", moderator);

		chan.enableInviteRestriction();
		Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(user);});
		Tester::assertFalse(chan.isConnected("Rabbit"));
		Tester::assertDoNotThrows([&]() {chan.inviteUser("Rabbit");});
		Tester::assertDoNotThrows([&]() {chan.connectUser(user);});
		Tester::assertTrue(chan.isConnected("Rabbit"));

	};

	void testPassword()
	{
		Tester::startTest("Channel password protection");

		User bob(2, dummyAddr(5678));
		bob.setNickname("bob");
		User alice(1, dummyAddr(1234));
		alice.setNickname("alice");
		Channel chan("secure", alice);

		Tester::assertDoNotThrows([&]() {chan.connectUser(bob, "wrongpass");});

		chan.setPassword("secret");

		Tester::assertThrows<PasswordMismatchException>([&]() {chan.connectUser(bob);});
		Tester::assertThrows<PasswordMismatchException>([&]() {chan.connectUser(bob, "wrongpass");});
		Tester::assertDoNotThrows([&]() {chan.connectUser(bob, "secret");});

		chan.disablePassword();

		Tester::assertDoNotThrows([&]() {chan.connectUser(bob, "secret");});
	};

	void testTopic()
	{
		Tester::startTest("Channel Topic restriction");
		User mod(2, dummyAddr(5678));
		mod.setNickname("mod");
		User user(2, dummyAddr(5678));
		user.setNickname("user");
		Channel chan("secure", mod);

		Tester::assertDoNotThrows([&]() {chan.setTopic("This is a test!", mod);});
		Tester::assertExpectedEqualsActual(std::string("This is a test!"), chan.getTopic());
		chan.enableTopicRestriction();
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {chan.setTopic("Changing topic error", user);});

	};

	void testUserLimit()
	{
		Tester::startTest("Channel User limit");

		User mod(1, dummyAddr(1001));
		mod.setNickname("mod");
		User u1(2, dummyAddr(1002));
		u1.setNickname("u1");
		User u2(3, dummyAddr(1003));
		u1.setNickname("u1");
		Channel chan("limited", mod);
		chan.setLimitNbUsers(2);

		Tester::assertDoNotThrows([&]() {chan.connectUser(u1);});
		Tester::assertThrows<ChannelIsFullException>([&]() {chan.connectUser(u2);});
	}
};