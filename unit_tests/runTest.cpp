#include "Tester.hpp"
#include "ChannelTester.hpp"
// #include "MsgServTester.hpp"
#include "UserTester.hpp"
#include "ExceptionTester.hpp"
#include "CommandTester.hpp"

void testerUser();

int main()
{
	{
		UserTester test;
		test.testerUser();
	}
	{
		ChannelTester test;
		test.testerChannel();
	}
	{
		// MsgServTester test;
		// test.testerMsgServ();
	}
	{
		CommandTester test;
		test.testerCommand();
	}
	{
		Tester::startTest("Init client");
		ExceptionTester test(4343, "password");
		test.testerException();
	}

}
