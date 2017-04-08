#ifndef TASK_ORDER_TEST_H
#define TASK_ORDER_TEST_H

#include <QString>
#include <QtTest/QtTest>

#include "model/model_a.h"

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

class TaskOrderTest : public QObject
{
	Q_OBJECT

public:
	TaskOrderTest() {}

private:
	SubscriptionManager* sm;
	TaskSchedulerMock  * scheduler;

	ModelA     * modelA;
	ImgModel   * imageModel;
	LabelsModel* labelsModel;
	DistModel  * distModel;

	void fakeSlot() {}

	void orderTestCore(QString dataId, std::vector<QString> expected,
	                   int waitMs = 250);

private Q_SLOTS:
	void init();
	void cleanup();

	void basicFlow();

	void image();
	void imageIMG();
	void imageNORM();
	void imageGRAD();
	void imageIMGPCA();
	void imageGRADPCA();
	void imageBRG();
	void imageRGB();

	void labels();
	void labelsIcons();

	void distIMG();
};

#endif // TASK_ORDER_TEST_H
