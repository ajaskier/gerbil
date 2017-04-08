#ifndef TASK_ORDER_TEST_H
#define TASK_ORDER_TEST_H

#include <QString>
#include <QtTest/QtTest>

#include "model/model_a.h"

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
	ModelA* modelA;
	SubscriptionManager* sm;
	TaskSchedulerMock  * scheduler;

	void fakeSlot() {}

private Q_SLOTS:
	void init();
	void cleanup();
	void flow1();
};

//QTEST_MAIN(TaskOrderTest)
//#include "moc_task_test.cpp"


#endif // TASK_ORDER_TEST_H
