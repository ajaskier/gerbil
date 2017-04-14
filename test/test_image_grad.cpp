#include "test_image_grad.h"

#include "task_scheduler_mock.h"

void TestImageGRAD::tst()
{
	orderTestCore("image.GRAD", dataTaskChain("image.GRAD"));
}


QTEST_MAIN(TestImageGRAD)
