#include "task_a.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"
#include <memory>
#include "lock.h"

TaskA::TaskA(int a)
	: Task("DATA_A", {}), a(a)
{}

TaskA::~TaskA()
{}

bool TaskA::run()
{
	//QThread::msleep(2000);

	Data data;
	data.num = a;
	Subscription::Lock<Data> lock(*sub("dest"));
	qDebug() << "got dataA";

	int version = lock.version();
	lock.setVersion(version + 1);
	lock.swap(data);

	qDebug() << "swapped";

	return true;
}
