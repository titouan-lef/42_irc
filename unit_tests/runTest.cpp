#include "Tester.hpp"
#include "ChannelTester.hpp"
#include "MsgServTester.hpp"
#include "UserTester.hpp"
#include "ExceptionTester.hpp"
#include "CommandTester.hpp"
#include "UserManagerTester.hpp"
#include "ModeCmdTester.hpp"
#include "PassCmdTester.hpp"
#include "NickCmdTester.hpp"

void testerUser();

int main()
{
	UserManager::getInstance().addNewUser(0, dummyAddr(4242)); //simulate server at user 0
	{
		UserTester test;
		test.testerUser();
	}
	{
		ChannelTester test;
		test.testerChannel();
	}
	{
		MsgServTester test;
		test.testerMsgServ();
	}
	{
		CommandTester test;
		test.testerCommand();
	}
	{
		UserManagerTester test;
		test.testerUserManager();
	}
	{
		PassCmdTester test;
		test.testerPassCmd();
	}
	{
		NickCmdTester test;
		test.testerNickCmd();
	}
	{
		ModeCmdTester test;
		test.testerModeCmd();
	}
	{
		Tester::startTest("Init client");
		ExceptionTester test(4343, "password");
		test.testerException();
	}

}
