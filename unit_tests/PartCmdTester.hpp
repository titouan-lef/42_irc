#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "PartCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

class PartCmdTester
{
public:
	void testerPartCmd()
	{
		testPermission();
		testPartChannels();
		/** test with messages */
	};

	void testPermission()
	{
		Tester::startTest("PART permission tests");

		PartCmd part;

		MsgServ::Command cmd = {"PART", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {part.execute(cmd ,10);});

		cmd = {"PART", "", {"#test", "#test2", "#test3"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {part.execute(cmd ,10);});

		cmd = {"PART", "trailing", {"#test"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {part.execute(cmd ,10);});
	};

	void testPartChannels()
	{
		Tester::startTest("PART channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		PartCmd part;

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		MsgServ::Command cmd = {"PART", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		ChanServ::getInstance().findOrCreateChannel("#chan", 10);
		cmd = {"PART", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		ChanServ::getInstance().findOrCreateChannel("#join", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		cmd = {"PART", "", {"#join"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));

		ChanServ::getInstance().findOrCreateChannel("#chan", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#chan", 10).connectUser(10);});
		cmd = {"PART", "", {"#join,#chan"}};
		Tester::assertDoNotThrows([&]() {part.execute(cmd ,10);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 1);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 1);});
		UserManager::getInstance().deleteUser(1);
	};
};