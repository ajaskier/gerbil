#include "test_image_rgb.h"

#include "task_scheduler_mock.h"

void TestImageRGB::tst()
{
	orderTestCore("image.rgb", dataTaskChain("image.rgb"));
}


QTEST_MAIN(TestImageRGB)
