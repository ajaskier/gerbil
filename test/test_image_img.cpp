#include "test_image_img.h"

#include "task_scheduler_mock.h"

void TestImageIMG::tst()
{
	orderTestCore("image.IMG", dataTaskChain("image.IMG"));
}


QTEST_MAIN(TestImageIMG)
