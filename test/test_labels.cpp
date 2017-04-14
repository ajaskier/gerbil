#include "test_labels.h"

#include "task_scheduler_mock.h"

void TestLabels::tst()
{
	orderTestCore("labels", dataTaskChain("labels"));
}


QTEST_MAIN(TestLabels)
