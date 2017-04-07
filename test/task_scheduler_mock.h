#ifndef TASK_SCHEDULER_MOCK_H
#define TASK_SCHEDULER_MOCK_H

#include "task_scheduler.h"
#include "task/task.h"

#include <QDebug>

class TaskSchedulerMock : public TaskScheduler {
	Q_OBJECT

public:

	TaskSchedulerMock(SubscriptionManager &sm) : TaskScheduler(sm) {}

	void pushTask(std::shared_ptr<Task> task)
	{
		qDebug() << "got task" << task->getId();

		TaskScheduler::pushTask(task);
	}

	void flushVector()
	{
		taskOrder.clear();
	}

	std::vector<QString> taskOrder;

private slots:
	void taskEnded(QString id, bool success)
	{
		qDebug() << "task ended" << id;

		taskOrder.push_back(id);
		TaskScheduler::taskEnded(id, success);
	}
};


#endif // TASK_SCHEDULER_MOCK_H
