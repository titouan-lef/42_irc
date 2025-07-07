#pragma once

#include "Tester.hpp"
#include "UserManagerTester.hpp"
#include "KickCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"

class KickCmdTester
{
public:
	void testerKickCmd()
	{
		testPermission();
		testKickChannels();
		/** test with messages */
	};

	void testPermission()
	{
		Tester::startTest("KICK permission tests");

		KickCmd kick;

		Command cmd = {"KICK", "", {}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", "", {"#test"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", "", {"#test", "#test2", "#test3", "reason"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", "trailing", {"#test"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", "", {"#test,#chan", "user"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});

		cmd = {"KICK", "", {"#test,#chan", "user,user1,user2"}};
		Tester::assertThrows<NeedMoreParamsException>([&]() {kick.execute(cmd ,10);});
	};

	void testKickChannels()
	{
		Tester::startTest("KICK users from channels");

		UserManager::getInstance().addNewUser(10, dummyAddr(4242));
		UserManager::getInstance().getUser(10).setNickname("Bob");
		KickCmd kick;

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		Command cmd = {"KICK", "", {"#join", "Bob"}};
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});

		ChanServ::getInstance().findOrCreateChannel("#join", 10);
		ChanServ::getInstance().findOrCreateChannel("#chan", 10);
		cmd = {"KICK", "", {"#join,#chan", "Bob,Bob"}};
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#chan", 1);});

		ChanServ::getInstance().findOrCreateChannel("#join", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		cmd = {"KICK", "", {"#join", "Bob"}};
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
	
		ChanServ::getInstance().findOrCreateChannel("#chan", 1);

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10).connectUser(10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#chan", 10).connectUser(10);});
		cmd = {"KICK", "", {"#join,#chan", "Bob,Bob"}};
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#join", 1).isConnected(10));
		Tester::assertFalse(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#join", "", 1);});

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#join", 10);});
		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().findOrCreateChannel("#chan", 10).connectUser(10);});
		cmd = {"KICK", "", {"#join,#chan", "Bob,Bob"}};
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,10);});
		Tester::assertThrows<NoSuchChannelException>([&]() {ChanServ::getInstance().findChannel("#join", 1);});
		Tester::assertTrue(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));
		Tester::assertDoNotThrows([&]() {kick.execute(cmd ,1);});
		Tester::assertFalse(ChanServ::getInstance().findChannel("#chan", 1).isConnected(10));

		Tester::assertDoNotThrows([&]() {ChanServ::getInstance().logOut("#chan", "", 1);});
		UserManager::getInstance().deleteUser(1);
	};
};