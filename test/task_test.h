#ifndef TASK_TEST_H
#define TASK_TEST_H

#include <QString>
#include <QtTest/QtTest>

#include "task_scheduler_mock.h"

class TaskTest : public QObject
{
	Q_OBJECT

public:
	TaskTest() {}

private:
	void fakeSlot() {}

private Q_SLOTS:
	void init();
	void cleanup();

	void basic();
};

//#include "moc_task_test.cpp"

#endif // TASK_TEST_H

