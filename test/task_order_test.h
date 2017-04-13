#ifndef TASK_ORDER_TEST_H
#define TASK_ORDER_TEST_H

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

class TaskOrderTest : public QObject
{
	Q_OBJECT

public:
	TaskOrderTest() {}

private:
	SubscriptionManager* sm;
	TaskSchedulerMock  * scheduler;

	ImgModel    * imageModel;
	LabelsModel * labelsModel;
	DistModel   * distModel;

	DataTaskChain dataTaskChain;

	void fakeSlot() {}

	void orderTestCore(QString dataId, std::vector<QString> expected,
	                   int waitMs = -1);

private Q_SLOTS:
	void init();
	void cleanup();

	void image();
	void imageIMG();
	void imageNORM();
	void imageGRAD();
	void imageIMGPCA();
	void imageGRADPCA();
	void imageRGB();
	void imageBRG();

	void labels();
	void labelsIcons();

	void distIMG();
	void distIMG_ROI_update();
	void distIMG_imageIMG_update();
};

#endif // TASK_ORDER_TEST_H
