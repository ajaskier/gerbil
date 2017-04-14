#include "test_dist_img_roi_update.h"

#include "task_scheduler_mock.h"

void TestDistIMG_ROIUpdate::tst()
{
	imageModel->setROI(cv::Rect(32, 32, 64, 64));
	std::vector<QString> expected;
	Subscription         * sub =
	    DataRegister::subscribe(Dependency("dist.IMG", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TestDistIMG_ROIUpdate::fakeSlot, this));

	QTest::qWait(1500);
	expected = { "ROI", "image", "image.IMG", "setLabels", "taskAdd" };
	QCOMPARE(scheduler->taskOrder, expected);
	scheduler->flushVector();

	imageModel->setROI(cv::Rect(32, 32, 64, 64));
	QTest::qWait(1500);
	sub->deleteLater();
	//delete sub;

	expected = { "ROI", "setLabels", "taskSub", "image.IMG", "taskAdd" };
	QCOMPARE(scheduler->taskOrder, expected);
}


QTEST_MAIN(TestDistIMG_ROIUpdate)
