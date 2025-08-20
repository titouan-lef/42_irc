#pragma once

#include "Tester.hpp"
#include "command.hpp"
#include "MsgServ.hpp"
#include "MessageTooLongException.hpp"

#include <iostream>

class MsgServTester
{
public:
	void testerMsgServ()
	{
		formatMsgReplyTest();
		formatNumericReplyTest();
		testCompleteMsgAndCommandExtraction();
		getCommandTest();
	}

private:
	void formatMsgReplyTest()
	{
		Tester::startTest("formatMsg() for message reply");
		{
			MsgServ::Message msg;
			msg.commandId = "COMMAND";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "myPrefix";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "COMMAND";
			msg.arg = {};
			msg.hasTrailing = false;
			msg.senderPrefix = "myPrefix";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "COMMAND";
			msg.arg = {};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "myPrefix";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "COMMAND";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = false;
			msg.senderPrefix = "myPrefix";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND arg1 arg2 arg3\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
	}

	void formatNumericReplyTest()
	{
		Tester::startTest("formatMsg() for numeric reply");
		{
			MsgServ::Message msg;
			msg.commandId = "423";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "423";
			msg.hasTrailing = false;
			msg.senderPrefix = "ircServer";
			
			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "002";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 002 arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "042";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 042 arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "040";
			msg.arg = {"arg1", "arg2", "arg3"};
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 040 arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "423";
			msg.hasTrailing = true;
			msg.trailing = "My trailing";
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
		{
			MsgServ::Message msg;
			msg.commandId = "423";
			msg.arg = {"arg1"};
			msg.hasTrailing = false;
			msg.senderPrefix = "ircServer";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 arg1\r\n"),
				MsgServ::getInstance().formatMsg(msg)
			);
		}
	}

	void testCompleteMsgAndCommandExtraction()
	{
		Tester::startTest("MsgServ complete message and command extraction");

		MsgServ& manager = MsgServ::getInstance();

		manager.addNewBuffer();
		manager.buffers_.at(0) += "PASS password\r\nNICK nick\r\n";
		Tester::assertTrue(manager.isCommandReady(0));
		std::string cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("PASS password"));
		Tester::assertTrue(manager.isCommandReady(0));
		cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("NICK nick"));

		manager.addNewBuffer();
		manager.buffers_.at(1) += "\r\n";
		Tester::assertTrue(manager.isCommandReady(1));
		cmd = manager.getCommand(1);
		Tester::assertExpectedEqualsActual(cmd, std::string(""));

		manager.buffers_.at(0) += "USER ";
		Tester::assertFalse(manager.isCommandReady(0));
		manager.buffers_.at(0) += "test ";
		Tester::assertFalse(manager.isCommandReady(0));
		manager.buffers_.at(0) += "0 * ";
		Tester::assertFalse(manager.isCommandReady(0));
		manager.buffers_.at(0) += ":This is a test\r\n";
		Tester::assertTrue(manager.isCommandReady(0));
		cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("USER test 0 * :This is a test"));
	}

	void getCommandTest()
	{
		Tester::startTest("MsgServ getCommand()");

		MsgServ& manager = MsgServ::getInstance();

		manager.addNewBuffer();
		manager.buffers_.at(0) += "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut ex ipsum, auctor sagittis arcu ut, venenatis varius orci. Pellentesque at enim elit. Nam a pulvinar quam. Proin ultricies ipsum ac semper lacinia. Aenean ligula justo, pellentesque vestibulum mi quis, consectetur venenatis eros. Phasellus aliquam metus sapien, ut vulputate eros egestas nec. Praesent facilisis vulputate mauris, at volutpat orci dignissim vel. Mauris dapibus consectetur augue vitae lobortis. Phasellus gravida enim eros, non porttitor\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Proin gravida hendrerit nulla quis blandit. Curabitur rutrum bibendum sem in eleifend. Nullam eleifend elit ut sem mattis, non lobortis sem ullamcorper. Donec ut nulla sodales, commodo urna et, ultrices leo. Duis aliquam tincidunt ante ut pellentesque. Vestibulum eu pretium risus. Sed ultricies rutrum ultrices. Proin condimentum nisi vel nisi commodo, ac mollis mi mattis. Maecenas nulla ipsum, tempus sagittis mi et, feugiat finibus orci. Donec tellus\n";
		Tester::assertTrue(manager.isCommandReady(0));
		Tester::assertThrows<MessageTooLongException>([&]() {manager.getCommand(0);});
		Tester::assertTrue(manager.isCommandReady(0));
		Tester::assertDoNotThrows([&]() {manager.getCommand(0);});
		std::string cmd = manager.getCommand(0);
	}
};
