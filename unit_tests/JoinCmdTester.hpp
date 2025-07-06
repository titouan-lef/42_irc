#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "JoinCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

class JoinCmdTester
{
public:
	void testerJoinCmd()
	{
		testPermission();
		testJoinChannels();
		testJoinPasswords();
	};

	void testPermission()
	{
		Tester::startTest("JOIN permission tests");

		JoinCmd join;

		MsgServ::Command cmd = {"JOIN", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", "", {"#test", "#test2", "#test3"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", "trailing", {"#test"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", "", {"test"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});

	};

	void testJoinChannels()
	{
		Tester::startTest("JOIN channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		JoinCmd join;

		MsgServ::Command cmd = {"JOIN", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});

		cmd = {"JOIN", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isModerator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		UserManager::getInstance().deleteUser(1);
	};

	void testJoinPasswords()
	{
		Tester::startTest("JOIN channels with passwords");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		ChanServ::getInstance().findOrCreateChannel("#join", 1);
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#join", 1).setPassword("password");});
		
		JoinCmd join;

		MsgServ::Command cmd = {"JOIN", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));

		cmd = {"JOIN", "", {"#join", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 10);});

		cmd = {"JOIN", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		cmd = {"JOIN", "", {"#join,#chan", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isModerator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		cmd = {"JOIN", "", {"#chan,#join", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isModerator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isModerator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});
		UserManager::getInstance().deleteUser(1);
	};
};