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
};
