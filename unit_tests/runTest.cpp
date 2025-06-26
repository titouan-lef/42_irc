#include "Tester.hpp"
#include "ChannelTester.hpp"
#include "MsgServTester.hpp"

void testerUser();

int main()
{
	testerUser();
	{
		ChannelTester test;
		test.testerChannel();
	}
	{
		MsgServTester test;
		test.testerMsgServ();
	}

}
