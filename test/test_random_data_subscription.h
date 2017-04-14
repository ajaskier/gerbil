#ifndef TEST_RANDOM_DATA_SUBSCRIPTION_H
#define TEST_RANDOM_DATA_SUBSCRIPTION_H

#include <QString>
#include <QtTest/QtTest>

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

#include "test_task_core.h"

class TestRandomDataSubscription : public TestTaskCore
{
	Q_OBJECT

public:
	TestRandomDataSubscription();

private:
	bool multiDataOrderRandomDelay(std::vector<QString> ids,
	                               std::vector<QString> expected);

	void fakeSlot() {}

	void printIds(std::vector<QString> ids);

	bool compareVectorsContent(std::vector<QString> v1,
	                           std::vector<QString> v2);

private Q_SLOTS:

	void rand_interval_tst();
};

#endif // TEST_RANDOM_DATA_SUBSCRIPTION_H
