#pragma once

#include "Tester.hpp"
#include "command.hpp"
#include "MsgServ.hpp"

#include <iostream>

class MsgServTester
{
public:
	void testerMsgServ()
	{
		formatMsgReplyTest();
		formatNumericReplyTest();
		testCompleteMsgAndCommandExtraction();
	}

private:
	void formatMsgReplyTest()
	{
		Tester::startTest("formatMsg() for message reply");
		{
			MsgServ::Message msg;
			msg.commandId = "COMMAND";
			msg.arg = {"arg1", "arg2", "arg3"};
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
			msg.trailing = "";
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
			msg.trailing = "";
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
			msg.arg = {};
			msg.trailing = "";
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
			msg.arg = {};
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
			msg.trailing = "";
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
		Tester::assertTrue(manager.completeMsg(0, "PASS password\r\nNICK nick\r\n") == 0);
		Tester::assertTrue(manager.isCommandReady(0));
		std::string cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("PASS password"));
		Tester::assertTrue(manager.isCommandReady(0));
		cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("NICK nick"));

		manager.addNewBuffer();
		Tester::assertTrue(manager.completeMsg(1, "\r\n") == 0);
		Tester::assertTrue(manager.isCommandReady(1));
		cmd = manager.getCommand(1);
		Tester::assertExpectedEqualsActual(cmd, std::string(""));

		Tester::assertTrue(manager.completeMsg(0, "USER ") == 0);
		Tester::assertFalse(manager.isCommandReady(0));
		Tester::assertTrue(manager.completeMsg(0, "test ") == 0);
		Tester::assertFalse(manager.isCommandReady(0));
		Tester::assertTrue(manager.completeMsg(0, "0 * ") == 0);
		Tester::assertFalse(manager.isCommandReady(0));
		Tester::assertTrue(manager.completeMsg(0, ":This is a test\r\n") == 0);
		Tester::assertTrue(manager.isCommandReady(0));
		cmd = manager.getCommand(0);
		Tester::assertExpectedEqualsActual(cmd, std::string("USER test 0 * :This is a test"));
	}
};
