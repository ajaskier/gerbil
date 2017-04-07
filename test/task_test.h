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
	void tst1()
	{
		std::vector<QString> expected = { "ala", "mala" };
		std::vector<QString> obtained = { "ala", "mala" };

		QCOMPARE(obtained, expected);
	}
};


#endif TASK_TEST_H

//QTEST_MAIN(TaskTest)
//#include "moc_task_order_test.cpp"
