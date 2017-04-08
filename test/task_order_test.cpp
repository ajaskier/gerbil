#include "task_order_test.h"

#include "task_scheduler_mock.h"

//in this scenario ROI is already computed
//before test suites are being run
void TaskOrderTest::init()
{
	sm = new SubscriptionManager();
	DataRegister::init(sm);
	scheduler = new TaskSchedulerMock(*sm);

	modelA      = new ModelA(1, scheduler, this);
	imageModel  = new ImgModel(false, scheduler, this);
	labelsModel = new LabelsModel(scheduler, this);
	imageModel->setFilename("/home/ocieslak/gerbil_data/fake_and_real_peppers_ms.txt");
	imageModel->setROI(cv::Rect(128, 128, 0, 0));

	QTest::qWait(250);
	scheduler->flushVector();
}

void TaskOrderTest::cleanup()
{
	delete scheduler;
	delete sm;
}

void TaskOrderTest::basicFlow()
{
	std::vector<QString> expected;
	Subscription         * dataASub =
	    DataRegister::subscribe(Dependency("DATA_A", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

	QTest::qWait(250);
	expected = { "DATA_A" };
	QCOMPARE(scheduler->taskOrder, expected);
	scheduler->flushVector();

	delete dataASub;
}

void TaskOrderTest::imageGRAD()
{
	std::vector<QString> expected;
	Subscription         * sub =
	    DataRegister::subscribe(Dependency("image.GRAD", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

	QTest::qWait(250);
	expected = { "image", "image.IMG", "image.GRAD" };
	QCOMPARE(scheduler->taskOrder, expected);
	scheduler->flushVector();

	delete sub;
}

QTEST_MAIN(TaskOrderTest)
