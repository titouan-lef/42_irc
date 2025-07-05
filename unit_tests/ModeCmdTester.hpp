#pragma once

#include "Tester.hpp"
#include "ModeCmd.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

class ModeCmdTester
{
public:
	void testerModeCmd()
	{
		ChanServ::getInstance().findOrCreateChannel(std::string("#test"), 1);
		Channel& chan = ChanServ::getInstance().findChannel("#test", 1);
		testPermission(chan);
		testOperator(chan);
		testInvite(chan);
		testTopic(chan);
		testPassword(chan);
		testLimit(chan);
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("MODE permission tests");

		ModeCmd mode;
		(void)chan;

		MsgServ::Command cmd = {"MODE", "", {"#test"}};
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {mode.execute(cmd ,10);});

		cmd = {"MODE", "", {"#testre"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,1);});

		cmd = {"MODE", "", {"test"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,1);});

		cmd = {"MODE", "", {"#test", "++o"}};
		Tester::assertThrows<UnknownModeException>([&]() {mode.execute(cmd ,1);});

		cmd = {"MODE", "", {"#test", "+"}};
		Tester::assertThrows<UnknownModeException>([&]() {mode.execute(cmd ,1);});
	};

	void testOperator(Channel& chan)
	{
		Tester::startTest("MODE operator tests");

		ModeCmd mode;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		UserManager::getInstance().getUser(10).setNickname("Alice");
		UserManager::getInstance().addNewUser(11, dummyAddr(4243));
		UserManager::getInstance().getUser(11).setNickname("Bob");
		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});

		Tester::assertFalse(chan.isModerator(10));
		Tester::assertFalse(chan.isModerator(11));

		MsgServ::Command cmd = {"MODE", "", {"#test", "+o", "Alice"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertTrue(chan.isModerator(10));

		cmd = {"MODE", "", {"#test", "-o", "Alice"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertFalse(chan.isModerator(10));

		cmd = {"MODE", "", {"#test", "+oo", "Alice", "Bob"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertTrue(chan.isModerator(10));
		Tester::assertTrue(chan.isModerator(11));

		cmd = {"MODE", "", {"#test", "-oo", "Alice", "Bob"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertFalse(chan.isModerator(10));
		Tester::assertFalse(chan.isModerator(11));

		cmd = {"MODE", "", {"#test", "+o"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});
		cmd = {"MODE", "", {"#test", "+oo", "Alice"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});
		cmd = {"MODE", "", {"#test", "-oo", "Alice"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});

		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(11);});
		UserManager::getInstance().deleteUser(1);
		UserManager::getInstance().deleteUser(1);
	};

	void testInvite(Channel& chan)
	{
		Tester::startTest("MODE invite tests");

		ModeCmd mode;

		MsgServ::Command cmd = {"MODE", "", {"#test", "+i"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(10);});

		cmd = {"MODE", "", {"#test", "-i"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});

		cmd = {"MODE", "", {"#test", "+ii"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertThrows<InviteOnlyException>([&]() {chan.connectUser(10);});

		cmd = {"MODE", "", {"#test", "-iii"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
	};

	void testTopic(Channel& chan)
	{
		Tester::startTest("MODE topic tests");

		ModeCmd mode;

		Tester::assertFalse(chan.isTopicRestricted());
		MsgServ::Command cmd = {"MODE", "", {"#test", "+t"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});

		cmd = {"MODE", "", {"#test", "-t"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertFalse(chan.isTopicRestricted());

		cmd = {"MODE", "", {"#test", "+tt"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertTrue(chan.isTopicRestricted());

		cmd = {"MODE", "", {"#test", "-ttt"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertFalse(chan.isTopicRestricted());
	};

	void testPassword(Channel& chan)
	{
		Tester::startTest("MODE password tests");

		ModeCmd mode;

		Tester::assertFalse(chan.isPasswordProtected());
		MsgServ::Command cmd = {"MODE", "", {"#test", "+k"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});

		Tester::assertFalse(chan.isPasswordProtected());
		cmd = {"MODE", "", {"#test", "+k", "password"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertTrue(chan.isPasswordProtected());

		Tester::assertThrows<BadChanMaskException>([&]() {chan.connectUser(10);});
		Tester::assertThrows<BadChanMaskException>([&]() {chan.connectUser(10, "wrongPassword");});
		Tester::assertDoNotThrows([&]() {chan.connectUser(10, "password");});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});

		cmd = {"MODE", "", {"#test", "-k"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertFalse(chan.isPasswordProtected());
		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
	};

	void testLimit(Channel& chan)
	{
		Tester::startTest("MODE limit tests");

		ModeCmd mode;

		Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, -1);
		MsgServ::Command cmd = {"MODE", "", {"#test", "+l"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});

		Tester::assertFalse(chan.isPasswordProtected());
		cmd = {"MODE", "", {"#test", "+l", "2"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, 2);

		Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
		Tester::assertThrows<ChannelIsFullException>([&]() {chan.connectUser(11);});

		cmd = {"MODE", "", {"#test", "-l"}};
		Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
		Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, -1);
		Tester::assertDoNotThrows([&]() {chan.connectUser(11);});

		Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
		Tester::assertDoNotThrows([&]() {chan.disconnectUser(11);});
	};
};