#include "test_image.h"

#include "task_scheduler_mock.h"

void TestImage::tst()
{
	orderTestCore("image", dataTaskChain("image"));
}


QTEST_MAIN(TestImage)
