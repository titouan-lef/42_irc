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

		Command cmd = {"JOIN", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", "", {"#test", "#test2", "#test3"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {join.execute(cmd ,1);});

		cmd = {"JOIN", "", {"test"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});

	};

	void testJoinChannels()
	{
		Tester::startTest("JOIN channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		JoinCmd join;

		Command cmd = {"JOIN", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});

		cmd = {"JOIN", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		UserManager::getInstance().deleteUser(1);
	};

	void testJoinPasswords()
	{
		Tester::startTest("JOIN channels with passwords");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		ChanServ::getInstance().findOrCreateChannel("#join", 1);
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findChannel("#join").setPassword("password");});
		
		JoinCmd join;

		Command cmd = {"JOIN", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));

		cmd = {"JOIN", "", {"#join", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 10);});

		cmd = {"JOIN", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		cmd = {"JOIN", "", {"#join,#chan", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		cmd = {"JOIN", "", {"#chan,#join", "password"}};
		Tester::assertDoNotThrows([&]() {join.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join").isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join").isOperator(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan").isOperator(10));
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", 10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan");});

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", 1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join");});
		UserManager::getInstance().deleteUser(1);
	};
};