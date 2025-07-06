#pragma once

#include "Tester.hpp"
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
			MsgServ::Command cmd;
			cmd.name = "COMMAND";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "My trailing";
			std::string prefix = "myPrefix";
			std::string target = "";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, target)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "COMMAND";
			cmd.arg = {};
			cmd.trailing = "";
			std::string prefix = "myPrefix";
			std::string target = "";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, target)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "COMMAND";
			cmd.arg = {};
			cmd.trailing = "My trailing";
			std::string prefix = "myPrefix";
			std::string target = "";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, target)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "COMMAND";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "";
			std::string prefix = "myPrefix";
			std::string target = "";

			Tester::assertExpectedEqualsActual(
				std::string(":myPrefix COMMAND arg1 arg2 arg3\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, target)
			);
		}
	}

	void formatNumericReplyTest()
	{
		Tester::startTest("formatMsg() for numeric reply");
		{
			MsgServ::Command cmd;
			cmd.name = "423";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "My trailing";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 targetClient arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "423";
			cmd.arg = {};
			cmd.trailing = "";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";
			
			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 targetClient\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "002";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "My trailing";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 002 targetClient arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "042";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "My trailing";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 042 targetClient arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "040";
			cmd.arg = {"arg1", "arg2", "arg3"};
			cmd.trailing = "My trailing";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 040 targetClient arg1 arg2 arg3 :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "423";
			cmd.arg = {};
			cmd.trailing = "My trailing";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 targetClient :My trailing\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
			);
		}
		{
			MsgServ::Command cmd;
			cmd.name = "423";
			cmd.arg = {"arg1"};
			cmd.trailing = "";
			std::string prefix = "ircServer";
			std::string clientTargetNickname = "targetClient";

			Tester::assertExpectedEqualsActual(
				std::string(":ircServer 423 targetClient arg1\r\n"),
				MsgServ::getInstance().formatMsg(cmd, prefix, clientTargetNickname)
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
