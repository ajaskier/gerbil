#include "test_image_bgr.h"

#include "task_scheduler_mock.h"

void TestImageBGR::tst()
{
	orderTestCore("image.bgr", dataTaskChain("image.bgr"));
}


QTEST_MAIN(TestImageBGR)
