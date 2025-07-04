#pragma once

#include "Tester.hpp"
#include "ModeCmd.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"

class ModeTester
{
public:
	void testerModeCmd()
	{
		ChanServ::getInstance().findOrCreateChannel(std::string("test"), 1);
		Channel& chan = ChanServ::getInstance().findChannel("test", 1);
		testPermission(chan);
		testOperator(chan);
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("MODE permission tests");

		ModeCmd mode;
		(void)chan;

		MsgServ::Command cmd = {"MODE", "", {"#test"}};
		Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {mode.execute(cmd ,10);});

		cmd = {"MODE", "", {"#testre"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,10);});

		cmd = {"MODE", "", {"test"}};
		Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,10);});

	};

	void testOperator(Channel& chan)
	{
		Tester::startTest("MODE operator tests");

		ModeCmd mode;
		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		UserManager::getInstance().getUser(10).setNickname("Alice");
		UserManager::getInstance().addNewUser(11, dummyAddr(4243));
		UserManager::getInstance().getUser(11).setNickname("Bob");
		chan.connectUser(10);

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

		chan.disconnectUser(10);
		chan.disconnectUser(11);
	};
};