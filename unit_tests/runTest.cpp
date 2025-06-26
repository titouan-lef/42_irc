#include "Tester.hpp"
#include "ChannelTester.hpp"

void testerUser();
void testerMsgServ();

int main()
{
	testerUser();
	ChannelTester test;
	test.testerChannel();
	testerMsgServ();
}
