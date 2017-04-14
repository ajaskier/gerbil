#include "test_dist_img_image_img_update.h"

#include "task_scheduler_mock.h"

void TestDistIMG_ImageIMGUpdate::tst()
{
	imageModel->setROI(cv::Rect(32, 32, 64, 64));
	std::vector<QString> expected;
	Subscription         * sub =
	    DataRegister::subscribe(Dependency("dist.IMG", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TestDistIMG_ImageIMGUpdate::fakeSlot, this));

	QTest::qWait(1500);
	expected = { "ROI", "image", "image.IMG", "setLabels", "taskAdd" };
	QCOMPARE(scheduler->taskOrder, expected);
	scheduler->flushVector();

	imageModel->delegateTask("image.IMG");
	QTest::qWait(1000);
	sub->deleteLater();

	//delete sub;

	expected = { "image.IMG", "taskAdd" };
	QCOMPARE(scheduler->taskOrder, expected);
}


QTEST_MAIN(TestDistIMG_ImageIMGUpdate)
