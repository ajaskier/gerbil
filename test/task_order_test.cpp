#include "task_order_test.h"

#include "task_scheduler_mock.h"

//in this scenario ROI is already computed
//before test suites are being run
void TaskOrderTest::init()
{
	sm = new SubscriptionManager(this);
	DataRegister::init(sm);
	scheduler = new TaskSchedulerMock(*sm, this);

	modelA      = new ModelA(1, scheduler, this);
	imageModel  = new ImgModel(false, scheduler, this);
	labelsModel = new LabelsModel(scheduler, this);
	distModel   = new DistModel(scheduler, this);

	imageModel->setFilename("/home/ocieslak/gerbil_data/fake_and_real_peppers_ms.txt");
	imageModel->setROI(cv::Rect(0, 0, 128, 128));
	imageModel->setBandsCount(31);

	labelsModel->setImageSize(cv::Size(128, 128));

	QTest::qWait(1000);
	scheduler->flushVector();
}

void TaskOrderTest::cleanup()
{
	scheduler->flushVector();

	scheduler->deleteLater();
	sm->deleteLater();
	QTest::qWait(250);
}

void TaskOrderTest::basicFlow()
{
	orderTestCore("DATA_A", { "DATA_A" });
}

void TaskOrderTest::orderTestCore(QString dataId, std::vector<QString> expected, int waitMs)
{
	Subscription * sub =
	    DataRegister::subscribe(Dependency(dataId, SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskOrderTest::fakeSlot, this));

	QTest::qWait(waitMs);
	QCOMPARE(scheduler->taskOrder, expected);
	delete sub;
}

void TaskOrderTest::image()
{
	orderTestCore("image", { "image" });
}

void TaskOrderTest::imageIMG()
{
	orderTestCore("image.IMG", { "image", "image.IMG" });
}

void TaskOrderTest::imageNORM()
{
	orderTestCore("image.NORM", { "image", "image.IMG", "image.NORM" });
}

void TaskOrderTest::imageGRAD()
{
	orderTestCore("image.GRAD", { "image", "image.IMG", "image.GRAD" });
}

void TaskOrderTest::imageIMGPCA()
{
	orderTestCore("image.IMGPCA", { "image", "image.IMG", "image.IMGPCA" }, 500);
}

void TaskOrderTest::imageGRADPCA()
{
	orderTestCore("image.GRADPCA", { "image", "image.IMG", "image.GRAD", "image.GRADPCA" }, 500);
}

void TaskOrderTest::imageRGB()
{
	orderTestCore("image.rgb", { "image", "image.bgr", "image.rgb" });
}

void TaskOrderTest::imageBRG()
{
	orderTestCore("image.bgr", { "image", "image.bgr" });
}

void TaskOrderTest::labels()
{
	orderTestCore("labels", { "setLabels" });
}

void TaskOrderTest::labelsIcons()
{
	orderTestCore("labels.icons", { "setLabels", "labels.icons" });
}

void TaskOrderTest::distIMG()
{
	orderTestCore("dist.IMG", { "image", "image.IMG", "setLabels", "taskAdd" });
}

QTEST_MAIN(TaskOrderTest)
