#ifndef TEST_IMAGE_RGB_H
#define TEST_IMAGE_RGB_H

#include <QString>
#include <QtTest/QtTest>

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

#include "test_task_core.h"

class TestImageRGB : public TestTaskCore
{
	Q_OBJECT

public:
	TestImageRGB() : TestTaskCore() {}

private:
	void fakeSlot() {}

private Q_SLOTS:

	void tst();
};

#endif // TEST_IMAGE_RGB_H
