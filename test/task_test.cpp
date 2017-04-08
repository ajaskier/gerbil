#include "task_test.h"

#include "task_scheduler_mock.h"

void TaskTest::init()
{
	sm = new SubscriptionManager();
	DataRegister::init(sm);
	scheduler = new TaskSchedulerMock(*sm);

	modelA = new ModelA(1, scheduler, this);
}

void TaskTest::cleanup()
{
	delete scheduler;
	delete sm;
}

void TaskTest::basic()
{
	std::vector<QString> expected;
	Subscription         * dataASub =
	    DataRegister::subscribe(Dependency("DATA_A", SubscriptionType::READ,
	                                       AccessType::DEFERRED), this,
	                            std::bind(&TaskTest::fakeSlot, this));

	QTest::qWait(250);
	expected = { "DATA_A" };
	QCOMPARE(scheduler->taskOrder, expected);
	scheduler->flushVector();

	delete dataASub;
}

//QTEST_MAIN(TaskTest)

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_Use96Dpi, true);
	QTEST_DISABLE_KEYPAD_NAVIGATION
	//QTEST_ADD_GPU_BLACKLIST_SUPPORT
	TaskTest tc;
	QTEST_SET_MAIN_SOURCE_PATH
	return QTest::qExec(&tc, argc, argv);
}
