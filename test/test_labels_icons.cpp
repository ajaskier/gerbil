#include "test_labels_icons.h"

#include "task_scheduler_mock.h"

void TestLabelsIcons::tst()
{
	orderTestCore("labels.icons", dataTaskChain("labels.icons"));
}


QTEST_MAIN(TestLabelsIcons)
