#ifndef TEST_IMAGE_H
#define TEST_IMAGE_H

#include <QString>
#include <QtTest/QtTest>

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

#include "test_task_core.h"

class TestImage : public TestTaskCore
{
	Q_OBJECT

public:
	TestImage() : TestTaskCore() {}

private:
	void fakeSlot() {}

private Q_SLOTS:

	void tst();
};

#endif // TEST_IMAGE_H
