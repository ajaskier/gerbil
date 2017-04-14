#ifndef TEST_MULTI_DATA_SUBSCRIPTIONS_H
#define TEST_MULTI_DATA_SUBSCRIPTIONS_H

#include <QString>
#include <QtTest/QtTest>

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

#include "test_task_core.h"

class TestMultiDataSubscriptions : public TestTaskCore
{
	Q_OBJECT

public:
	TestMultiDataSubscriptions();

private:
	void multiDataOrderTest(std::vector<QString> ids,
	                        MultiDataChainResult expected);


	void fakeSlot() {}

	void printIds(std::vector<QString> ids);


private Q_SLOTS:

	void tst();
};

#endif // TEST_MULTI_DATA_SUBSCRIPTIONS_H
