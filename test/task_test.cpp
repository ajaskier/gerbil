#include "task_test.h"

#include "task_scheduler_mock.h"

void TaskTest::init()
{}

void TaskTest::cleanup()
{}

void TaskTest::basic()
{
	QCOMPARE("test", "test2");
}

QTEST_MAIN(TaskTest)
