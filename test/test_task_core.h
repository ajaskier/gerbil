#ifndef TEST_TASK_CORE_H
#define TEST_TASK_CORE_H

#include <QString>
#include <QtTest/QtTest>

#include "model/img_model.h"
#include "model/dist_model.h"
#include "model/labels_model.h"
#include "model/clusterization_model.h"
#include "model/falsecolor_model.h"
#include "model/graph_seg_model.h"

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

class TestTaskCore : public QObject
{
	Q_OBJECT

public:
	TestTaskCore() {}

protected:
	SubscriptionManager* sm;
	TaskSchedulerMock  * scheduler;

	ImgModel      * imageModel;
	LabelsModel   * labelsModel;
	DistModel     * distModel;
	DataTaskChain dataTaskChain;

	void fakeSlot() {}

	void orderTestCore(QString dataId, std::vector<QString> expected,
	                   int waitMs = -1)
	{
		if (std::find(dataTaskChain(dataId).begin(), dataTaskChain(dataId).end(),
		              "ROI") != dataTaskChain(dataId).end()) {
			imageModel->setROI(cv::Rect(0, 0, 128, 128));
			QTest::qWait(100);
		}

		Subscription * sub =
		    DataRegister::subscribe(Dependency(dataId, SubscriptionType::READ,
		                                       AccessType::DEFERRED), this,
		                            std::bind(&TestTaskCore::fakeSlot, this));

		if (waitMs == -1) {
			waitMs = 150 * dataTaskChain(dataId).size();
		}
		QTest::qWait(waitMs);
		sub->deleteLater();
		QCOMPARE(scheduler->taskOrder, expected);
	}

protected Q_SLOTS:
	void init()
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

	void cleanup()
	{
		scheduler->flushVector();
		scheduler->deleteLater();
		sm->deleteLater();
		QTest::qWait(250);
	}
};

#endif // TEST_TASK_CORE_H
