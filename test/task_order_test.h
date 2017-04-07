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
	void init()
	{
		sm = new SubscriptionManager();
		DataRegister::init(sm);
		scheduler = new TaskSchedulerMock(*sm);

		modelA = new ModelA(1, scheduler, this);
	}

	void cleanup()
	{
		delete modelA;
		delete scheduler;
		delete sm;
	}

	void flow1()
	{
		std::vector<QString> expected;
		Subscription         * dataASub =
		    DataRegister::subscribe(Dependency("DATA_A", SubscriptionType::READ,
		                                       AccessType::DEFERRED),
		                            this,
		                            std::bind(&TaskOrderTest::
		                                      fakeSlot, this));

		QTest::qWait(250);
		expected = { "DATA_A" };
		QCOMPARE(scheduler->taskOrder, expected);
		scheduler->flushVector();

		delete dataASub;
	}
};

//QTEST_MAIN(TaskOrderTest)
//#include "moc_task_test.cpp"


#endif // TASK_ORDER_TEST_H
