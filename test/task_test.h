#ifndef TASK_TEST_H
#define TASK_TEST_H

#include <QString>
#include <QtTest/QtTest>

class TaskTest : public QObject
{
	Q_OBJECT

public:
	TaskTest() {}

private:
	void fakeSlot() {}

private Q_SLOTS:
	void init() {}
	void cleanup() {}
	void tst1();
};


//#include "moc_task_order_test.cpp"
#endif // TASK_TEST_H

