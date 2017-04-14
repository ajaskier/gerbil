#ifndef TEST_DIST_IMG_ROI_UPDATE_H
#define TEST_DIST_IMG_ROI_UPDATE_H

#include <QString>
#include <QtTest/QtTest>

#include "subscription_manager.h"
#include "subscription.h"
#include "data_register.h"

#include "task_scheduler_mock.h"
#include "data_task_chain.h"

#include "test_task_core.h"

class TestDistIMG_ROIUpdate : public TestTaskCore
{
	Q_OBJECT

public:
	TestDistIMG_ROIUpdate() : TestTaskCore() {}

private:
	void fakeSlot() {}

private Q_SLOTS:

	void tst();
};

#endif // TEST_DIST_IMG_ROI_UPDATE_H
