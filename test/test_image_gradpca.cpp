#include "test_image_gradpca.h"

#include "task_scheduler_mock.h"

void TestImageGRADPCA::tst()
{
	orderTestCore("image.GRADPCA", dataTaskChain("image.GRADPCA"));
}


QTEST_MAIN(TestImageGRADPCA)
