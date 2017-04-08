#include "task_order_test.h"

#include "task_scheduler_mock.h"


void TaskOrderTest::init()
{
	sm = new SubscriptionManager();
	DataRegister::init(sm);
	scheduler = new TaskSchedulerMock(*sm);

	modelA = new ModelA(1, scheduler, this);
}

void TaskOrderTest::cleanup()
{
	delete modelA;
	delete scheduler;
	delete sm;
}

void TaskOrderTest::flow1()
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

QTEST_MAIN(TaskOrderTest)
