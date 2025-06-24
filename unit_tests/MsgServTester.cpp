#include "Tester.hpp"

#include "MsgServ.hpp"

#include <iostream>

static void getMessageReplyTest()
{
	Tester::startTest("getMessageReply()");
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "My trailing";
		std::string clientNickname = "clientNickname";

		Tester::assertExpectedEqualsActual(
			std::string(":clientNickname COMMAND arg1 arg2 arg3 :My trailing\r\n"),
			MsgServ::getMessageReply(cmd, clientNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {};
		cmd.trailing = "";
		std::string clientNickname = "clientNickname";

		Tester::assertExpectedEqualsActual(
			std::string(":clientNickname COMMAND\r\n"),
			MsgServ::getMessageReply(cmd, clientNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {};
		cmd.trailing = "My trailing";
		std::string clientNickname = "clientNickname";

		Tester::assertExpectedEqualsActual(
			std::string(":clientNickname COMMAND :My trailing\r\n"),
			MsgServ::getMessageReply(cmd, clientNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "";
		std::string clientNickname = "clientNickname";

		Tester::assertExpectedEqualsActual(
			std::string(":clientNickname COMMAND arg1 arg2 arg3\r\n"),
			MsgServ::getMessageReply(cmd, clientNickname)
		);
	}
}

static void getNumericReplyTest()
{
	Tester::startTest("getNumericReply()");
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "My trailing";
		int code = 423;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 423 targetClient arg1 arg2 arg3 :My trailing\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {};
		cmd.trailing = "";
		int code = 423;
		std::string clientTargetNickname = "targetClient";
		
		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 423 targetClient\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "My trailing";
		int code = 2;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 002 targetClient arg1 arg2 arg3 :My trailing\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "My trailing";
		int code = 42;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 042 targetClient arg1 arg2 arg3 :My trailing\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1", "arg2", "arg3"};
		cmd.trailing = "My trailing";
		int code = 40;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 040 targetClient arg1 arg2 arg3 :My trailing\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {};
		cmd.trailing = "My trailing";
		int code = 423;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 423 targetClient :My trailing\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
	{
		MsgServ::Command cmd;
		cmd.name = "COMMAND";
		cmd.arg = {"arg1"};
		cmd.trailing = "";
		int code = 423;
		std::string clientTargetNickname = "targetClient";

		Tester::assertExpectedEqualsActual(
			std::string(":ircServer 423 targetClient arg1\r\n"),
			MsgServ::getNumericReply(cmd, code, clientTargetNickname)
		);
	}
}



/**
 * @brief Function which execute all tests.
 */
void testerMsgServ()
{
	getMessageReplyTest();
	getNumericReplyTest();

	/** @todo defines other tests */
}