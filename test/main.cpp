
#include <QApplication>
#include <QDebug>
#include <QTest>

#include "task_order_test.h"
#include "task_test.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	TaskTest      t1;
	TaskOrderTest t2;

	QTest::qExec(&t1, argc, argv);
	QTest::qExec(&t2);

//	return app.exec();
}
