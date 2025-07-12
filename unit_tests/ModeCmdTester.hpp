#pragma once

#include "Tester.hpp"
#include "ChanServ.hpp"
#include "ModeCmd.hpp"
#include "UserNotInChannelException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchChannelException.hpp"
#include "NeedMoreParamsException.hpp"
#include "InviteOnlyException.hpp"
#include "UnknownModeException.hpp"
#include "ChannelIsFullException.hpp"
#include "BadChanKeyException.hpp"
#include "command.hpp"

class ModeCmdTester
{
public:
	void testerModeCmd()
	{
		ChanServ::getInstance().findOrCreateChannel(std::string("#test"), 1);
		Channel& chan = ChanServ::getInstance().findChannel("#test");
		testPermission(chan);
		testOperator(chan);
		testInvite(chan);
		testTopic(chan);
		testPassword(chan);
		testLimit(chan);
		testerModeReply();
	};

	void testPermission(Channel& chan)
	{
		Tester::startTest("MODE permission tests");
		{
			ModeCmd mode;

			Command cmd = {"MODE", "", {}};
			Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,10);});
			
			cmd = {"MODE", "", {"#test"}};
			Tester::assertThrows<UserNotInChannelException>([&]() {mode.execute(cmd ,10);});

			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
			
			cmd = {"MODE", "", {"#test"}};
			Tester::assertThrows<ChannelPrivilegesNeeded>([&]() {mode.execute(cmd ,10);});

			cmd = {"MODE", "", {"#testre"}};
			Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,1);});

			cmd = {"MODE", "", {"test"}};
			Tester::assertThrows<NoSuchChannelException>([&]() {mode.execute(cmd ,1);});

			cmd = {"MODE", "", {"#test", "++o"}};
			Tester::assertThrows<UnknownModeException>([&]() {mode.execute(cmd ,1);});

			cmd = {"MODE", "", {"#test", "+"}};
			Tester::assertThrows<UnknownModeException>([&]() {mode.execute(cmd ,1);});

			ChanServ::getInstance().partAllChannels(10);
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("MODE");
			user1.assertReply(":ircserv 461 user1 MODE :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user2.sendCommand("MODE #test");
			user2.assertReply(":ircserv 441 user2 user2 #test :They aren't on that channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user2.skipReplies(4);
			user2.sendCommand("MODE #test");
			user2.assertReply(":ircserv 482 user2 #test :You're not channel operator");
		}
	};

	void testOperator(Channel& chan)
	{
		Tester::startTest("MODE operator tests");
		{
			ModeCmd mode;
			UserManager::getInstance().addNewUser(10, dummyAddr(4242));
			UserManager::getInstance().getUser(10).setNickname("Alice");
			UserManager::getInstance().addNewUser(11, dummyAddr(4243));
			UserManager::getInstance().getUser(11).setNickname("Bob");
			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
			Tester::assertDoNotThrows([&]() {chan.connectUser(11);});

			Tester::assertFalse(chan.isOperator(10));
			Tester::assertFalse(chan.isOperator(11));

			Command cmd = {"MODE", "", {"#test", "+o", "Alice"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertTrue(chan.isOperator(10));

			cmd = {"MODE", "", {"#test", "-o", "Alice"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertFalse(chan.isOperator(10));

			cmd = {"MODE", "", {"#test", "+oo", "Alice", "Bob"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertTrue(chan.isOperator(10));
			Tester::assertTrue(chan.isOperator(11));

			cmd = {"MODE", "", {"#test", "-oo", "Alice", "Bob"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertFalse(chan.isOperator(10));
			Tester::assertFalse(chan.isOperator(11));

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
		}
		Tester::startTest("MODE operator reply tests");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +o");
			user1.assertReply(":ircserv 461 user1 MODE :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +o Bob");
			user1.assertReply(":ircserv 401 user1 Bob :No such nick/channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +oo Bob");
			user1.assertReply(":ircserv 461 user1 MODE :Not enough parameters");
		}
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test -o user1");
			user1.assertReply(":user1!user1@localhost MODE #test -o user1");
			user1.sendCommand("MODE #test +o user1");
			user1.assertReply(":ircserv 482 user1 #test :You're not channel operator");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +o user2");
			user1.assertReply(":ircserv 441 user1 user2 #test :They aren't on that channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("MODE #test +o user2");
			user1.assertReply(":user1!user1@localhost MODE #test +o user2");
			user2.sendCommand("MODE #test -o user1");
			user1.assertReply(":user2!user2@localhost MODE #test -o user1");
			user1.sendCommand("MODE #test +o user2");
			user1.assertReply(":ircserv 482 user1 #test :You're not channel operator");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("MODE #test +oo user2 user3");
			user1.assertReply(":ircserv 401 user1 user3 :No such nick/channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);
			user1.skipReplies(5);
			user1.sendCommand("MODE #test -oo user1 user3");
			user1.assertReply(":ircserv 401 user1 user3 :No such nick/channel");
			user1.assertReply(":user1!user1@localhost MODE #test -o user1");

			user1.sendCommand("MODE #test +o user2");
			user1.assertReply(":ircserv 482 user1 #test :You're not channel operator");
		}
	};

	void testInvite(Channel& chan)
	{
		Tester::startTest("MODE invite tests");
		{
			ModeCmd mode;

			Command cmd = {"MODE", "", {"#test", "+i"}};
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
		}
		Tester::startTest("MODE invite reply tests");
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +i");
			user1.assertReply(":user1!user1@localhost MODE #test +i");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +i");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 473 user2 #test :You're not invited to join this channel");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +i");
			user1.assertReply(":user1!user1@localhost MODE #test +i");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +i");

			user1.sendCommand("INVITE user2 #test");
			user2.sendCommand("JOIN #test");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);
			user1.sendCommand("MODE #test +i");
			user1.assertReply(":user1!user1@localhost MODE #test +i");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +i");

			user1.sendCommand("INVITE user2 #test");
			user2.sendCommand("JOIN #test");
			user1.skipReplies(1);
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");

			user2.sendCommand("PART #test");
			user1.skipReplies(1);
			user2.skipReplies(1);

			user1.sendCommand("MODE #test -i");
			user1.assertReply(":user1!user1@localhost MODE #test -i");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
		}
	};

	void testTopic(Channel& chan)
	{
		Tester::startTest("MODE topic tests");
		{
			ModeCmd mode;

			Tester::assertFalse(chan.isTopicRestricted());
			Command cmd = {"MODE", "", {"#test", "+t"}};
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
		}
		Tester::startTest("MODE topic reply tests");
		{
			ReplyTester user1(4343, "password", "user1");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +t");
			user1.assertReply(":user1!user1@localhost MODE #test +t");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +t");

			user1.sendCommand("TOPIC #test :Changing topic");
			user1.assertReply(":user1!user1@localhost TOPIC #test :Changing topic");

			user1.sendCommand("LIST #test");
			user1.assertReply(":ircserv 322 user1 #test 1 :Changing topic");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +t");
			user1.assertReply(":user1!user1@localhost MODE #test +t");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +t");

			user2.sendCommand("TOPIC #test :Changing topic");
			user2.assertReply(":ircserv 441 user2 user2 #test :They aren't on that channel");

			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);

			user2.sendCommand("TOPIC #test :Changing topic");
			user2.assertReply(":ircserv 482 user2 #test :You're not channel operator");

			user2.sendCommand("LIST #test");
			user2.assertReply(":ircserv 322 user2 #test 2");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +t");
			user1.assertReply(":user1!user1@localhost MODE #test +t");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +t");

			user2.sendCommand("JOIN #test");
			user2.skipReplies(4);

			user2.sendCommand("TOPIC #test :Changing topic");
			user2.assertReply(":ircserv 482 user2 #test :You're not channel operator");

			user2.sendCommand("LIST #test");
			user2.assertReply(":ircserv 322 user2 #test 2");
		}

	};

	void testPassword(Channel& chan)
	{
		Tester::startTest("MODE password tests");
		{
			ModeCmd mode;

			Tester::assertFalse(chan.isPasswordProtected());
			Command cmd = {"MODE", "", {"#test", "+k"}};
			Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});

			Tester::assertFalse(chan.isPasswordProtected());
			cmd = {"MODE", "", {"#test", "+k", "password"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertTrue(chan.isPasswordProtected());

			Tester::assertThrows<BadChanKeyException>([&]() {chan.connectUser(10);});
			Tester::assertThrows<BadChanKeyException>([&]() {chan.connectUser(10, "wrongPassword");});
			Tester::assertDoNotThrows([&]() {chan.connectUser(10, "password");});
			Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});

			cmd = {"MODE", "", {"#test", "-k"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertFalse(chan.isPasswordProtected());
			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
			Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
		}
		Tester::startTest("MODE password reply test");
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +k password");
			user1.assertReply(":user1!user1@localhost MODE #test +k password");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +k");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 476 user2 #test :Cannot join channel (+k)");
		}
		{
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +k password");
			user1.assertReply(":user1!user1@localhost MODE #test +k password");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +k");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 476 user2 #test :Cannot join channel (+k)");

			user2.sendCommand("JOIN #test password");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
			user1.skipReplies(1);

			user2.sendCommand("PART #test");
			user2.assertReply(":user2!user2@localhost PART #test");
			user1.skipReplies(1);

			user1.sendCommand("MODE #test -k");
			user1.assertReply(":user1!user1@localhost MODE #test -k");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
		}
	};

	void testLimit(Channel& chan)
	{
		{
			Tester::startTest("MODE limit tests");

			ModeCmd mode;

			Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, (std::size_t)0);
			Command cmd = {"MODE", "", {"#test", "+l"}};
			Tester::assertThrows<NeedMoreParamsException>([&]() {mode.execute(cmd ,1);});

			Tester::assertFalse(chan.isPasswordProtected());
			cmd = {"MODE", "", {"#test", "+l", "2"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, (std::size_t)2);

			Tester::assertDoNotThrows([&]() {chan.connectUser(10);});
			Tester::assertThrows<ChannelIsFullException>([&]() {chan.connectUser(11);});

			cmd = {"MODE", "", {"#test", "-l"}};
			Tester::assertDoNotThrows([&]() {mode.execute(cmd ,1);});
			Tester::assertExpectedEqualsActual(chan.nbMaxUsers_, (std::size_t)0);
			Tester::assertDoNotThrows([&]() {chan.connectUser(11);});

			Tester::assertDoNotThrows([&]() {chan.disconnectUser(10);});
			Tester::assertDoNotThrows([&]() {chan.disconnectUser(11);});
		}
		{
			Tester::startTest("MODE limit relply test");
			
			ReplyTester user1(4343, "password", "user1");
			ReplyTester user2(4343, "password", "user2");

			user1.sendCommand("JOIN #test");
			user1.skipReplies(4);

			user1.sendCommand("MODE #test +l 1");
			user1.assertReply(":user1!user1@localhost MODE #test +l 1");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test +l 1");

			user2.sendCommand("JOIN #test");
			user2.assertReply(":ircserv 471 user2 #test :Cannot join channel (+l)");

			user1.sendCommand("MODE #test -l");
			user1.assertReply(":user1!user1@localhost MODE #test -l");

			user1.sendCommand("MODE #test");
			user1.assertReply(":ircserv 324 user1 #test");

			user2.sendCommand("JOIN #test password");
			user2.assertReply(":user2!user2@localhost JOIN :#test");
			user2.assertReply(":ircserv 331 user2 #test :No topic is set");
			user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
			user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
		}
	};

	void testerModeReply()
	{
		Tester::startTest("MODE replies test");
		
		ReplyTester user1(4343, "password", "user1");
		ReplyTester user2(4343, "password", "user2");

		user1.sendCommand("JOIN #test");
		user1.skipReplies(4);
		user1.sendCommand("MODE #test +it");
		user1.assertReply(":user1!user1@localhost MODE #test +it");

		user1.sendCommand("MODE #test");
		user1.assertReply(":ircserv 324 user1 #test +it");

		user1.sendCommand("MODE #test +k password");
		user1.assertReply(":user1!user1@localhost MODE #test +k password");

		user1.sendCommand("MODE #test");
		user1.assertReply(":ircserv 324 user1 #test +ikt");

		user1.sendCommand("MODE #test +l 1");
		user1.assertReply(":user1!user1@localhost MODE #test +l 1");

		user1.sendCommand("MODE #test");
		user1.assertReply(":ircserv 324 user1 #test +iktl 1");

		user2.sendCommand("JOIN #test");
		user2.assertReply(":ircserv 476 user2 #test :Cannot join channel (+k)");

		user2.sendCommand("JOIN #test password");
		user2.assertReply(":ircserv 473 user2 #test :You're not invited to join this channel");

		user1.sendCommand("INVITE user2 #test");
		user2.sendCommand("JOIN #test password");
		user2.assertReply(":ircserv 471 user2 #test :Cannot join channel (+l)");

		user1.sendCommand("MODE #test -l");
		user1.assertReply(":user1!user1@localhost MODE #test -l");

		user1.sendCommand("MODE #test");
		user1.assertReply(":ircserv 324 user1 #test +ikt");

		user2.sendCommand("JOIN #test password");
		user2.assertReply(":user2!user2@localhost JOIN :#test");
		user2.assertReply(":ircserv 331 user2 #test :No topic is set");
		user2.assertReply(":ircserv 353 user2 = #test :@user1 user2");
		user2.assertReply(":ircserv 366 user2 #test :End of NAMES list");
	};
};