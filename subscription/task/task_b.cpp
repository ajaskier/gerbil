#include "task_b.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskB::TaskB(int b)
	: Task("DATA_B", { { "source", { "DATA_A" } } }), b(b)
{}

TaskB::~TaskB()
{}

bool TaskB::run()
{
	Data data;
	//read phase
	{
		Subscription::Lock<Data> lock(*sub("source"));
		data.num = b + lock()->num;
		QThread::msleep(2000);
		//QThread::sleep(5);
	}
	//write phase
	{
		Subscription::Lock<Data> lock(*sub("dest"));
		lock.swap(data);
	}

	return true;
}
