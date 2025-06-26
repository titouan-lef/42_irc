#include "Tester.hpp"
#include "ChannelTester.hpp"
#include "MsgServTester.hpp"
#include "UserTester.hpp"

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
		MsgServTester test;
		test.testerMsgServ();
	}

}
