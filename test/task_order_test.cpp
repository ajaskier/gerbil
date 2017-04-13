#include "task_order_test.h"

#include "task_scheduler_mock.h"

//in this scenario ROI is already computed
//before test suites are being run
void TaskOrderTest::init()
{
	sm = new SubscriptionManager(this);
	DataRegister::init(sm);
	scheduler = new TaskSchedulerMock(*sm, this);

	imageModel  = new ImgModel(false, scheduler, this);
	labelsModel = new LabelsModel(scheduler, this);
	distModel   = new DistModel(scheduler, this);

	imageModel->setFilename("/home/ocieslak/gerbil_data/fake_and_real_peppers_ms.txt");
	imageModel->setBandsCount(31);
	labelsModel->setImageSize(cv::Size(128, 128));
}

void TaskOrderTest::cleanup()
{
	imageModel->deleteLater();
	labelsModel->deleteLater();
	distModel->deleteLater();

	scheduler->deleteLater();
	sm->deleteLater();
	QTest::qWait(500);
}

void TaskOrderTest::orderTestCore(QString dataId, std::vector<QString> expected, int waitMs)
{
	qDebug() << "requested" << dataId;

	if (std::find(expected.begin(), expected.end(), "ROI") != expected.end()) {
		imageModel->setROI(cv::Rect(0, 0, 128, 128));
		QTest::qWait(100);
	}

	Subscription * sub =
	    DataRegister::subscribe(Dependency(dataId, SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

	if (waitMs == -1) {
		waitMs = 150 * expected.size();
	}
	QTest::qWait(waitMs);
	sub->deleteLater();
	//delete sub;

	QCOMPARE(scheduler->taskOrder, expected);
}

void TaskOrderTest::image()
{
	orderTestCore("image", dataTaskChain("image"));
}

void TaskOrderTest::imageIMG()
{
	orderTestCore("image.IMG", dataTaskChain("image.IMG"));
}

void TaskOrderTest::imageNORM()
{
	orderTestCore("image.NORM", dataTaskChain("image.NORM"));
}

void TaskOrderTest::imageGRAD()
{
	orderTestCore("image.GRAD", dataTaskChain("image.GRAD"), 500);
}

void TaskOrderTest::imageIMGPCA()
{
	orderTestCore("image.IMGPCA", dataTaskChain("image.IMGPCA"), 500);
}

void TaskOrderTest::imageGRADPCA()
{
	orderTestCore("image.GRADPCA", dataTaskChain("image.GRADPCA"), 500);
}

void TaskOrderTest::imageRGB()
{
	orderTestCore("image.rgb", dataTaskChain("image.rgb"));
}

void TaskOrderTest::imageBRG()
{
	orderTestCore("image.bgr", dataTaskChain("image.bgr"));
}

void TaskOrderTest::labels()
{
	orderTestCore("labels", dataTaskChain("labels"), 500);
}

void TaskOrderTest::labelsIcons()
{
	orderTestCore("labels.icons", dataTaskChain("labels.icons"), 500);
}

void TaskOrderTest::distIMG()
{
	orderTestCore("dist.IMG", dataTaskChain("dist.IMG"), 750);
}

void TaskOrderTest::distIMG_ROI_update()
{
	imageModel->setROI(cv::Rect(32, 32, 64, 64));
	std::vector<QString> expected;
	Subscription         * sub =
	    DataRegister::subscribe(Dependency("dist.IMG", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

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

void TaskOrderTest::distIMG_imageIMG_update()
{
	imageModel->setROI(cv::Rect(32, 32, 64, 64));
	std::vector<QString> expected;
	Subscription         * sub =
	    DataRegister::subscribe(Dependency("dist.IMG", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

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

QTEST_MAIN(TaskOrderTest)
