#include "test_dist_img.h"

#include "task_scheduler_mock.h"

void TestDistIMG::tst()
{
	orderTestCore("dist.IMG", dataTaskChain("dist.IMG"));
}


QTEST_MAIN(TestDistIMG)
