#include "task_test.h"


void TaskTest::tst1()
{
	std::vector<QString> expected = { "ala", "mala" };
	std::vector<QString> obtained = { "ala", "mala" };

	QCOMPARE(obtained, expected);
}

QTEST_MAIN(TaskTest)
//#include "moc_task_order_test.cpp"
