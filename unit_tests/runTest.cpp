#include "Tester.hpp"

static void test1()
{
	Tester::startTest(/** @todo defines test name if you want */);
	{
		/** @todo defines sub test 1 */
	}
	{
		/** @todo defines sub test 2 */
	}
}

/**
 * @brief Function which execute all tests.
 * @details For each test, you can use functions like :
 * Tester::assertExpectedEqualsActual(expected, actual)
 * Tester::assertExpectedNotEqualsActual(expected, actual)
 * Tester::assertTrue(condition)
 * Tester::assertFalse(condition)
 * Tester::assertNull(ptr)
 * Tester::assertNotNull(ptr)
 */
void runTest()
{
	test1();
	/** @todo defines other tests */
}
