#include "test_image_imgpca.h"

#include "task_scheduler_mock.h"

void TestImageIMGPCA::tst()
{
	orderTestCore("image.IMGPCA", dataTaskChain("image.IMGPCA"));
}


QTEST_MAIN(TestImageIMGPCA)
