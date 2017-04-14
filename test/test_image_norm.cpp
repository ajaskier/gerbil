#include "test_image_norm.h"

#include "task_scheduler_mock.h"

void TestImageNORM::tst()
{
	orderTestCore("image.NORM", dataTaskChain("image.NORM"));
}


QTEST_MAIN(TestImageNORM)
