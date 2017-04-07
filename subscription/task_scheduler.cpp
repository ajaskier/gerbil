#include "task_scheduler.h"
#include "subscription_manager.h"
#include "model/model.h"
#include "task/task.h"
#include "worker_thread.h"
#include "subscription.h"
#include "data_register.h"
#include <QDebug>

TaskScheduler::TaskScheduler(SubscriptionManager &sm) : QObject(), sm(sm) {}

void TaskScheduler::pushTask(std::shared_ptr<Task> task)
{
	createSubscriptions(task);

	removeDuplicates(task->getId());

	taskPool.push_front(task);
	checkTaskPool();
}

void TaskScheduler::printPool()
{
	for (auto task : taskPool)
		qDebug() << "in pool: " << task->getId();

	for (auto it : runningTasks)
		qDebug() << "currently running: " << it.second->getId();
}

void TaskScheduler::createSubscriptions(std::shared_ptr<Task> task)
{
	for (auto& dependency : task->getDependencies()) {
		std::shared_ptr<Subscription> s(DataRegister::subscribe(dependency));
		task->importSubscription(std::move(s));
	}
}

void TaskScheduler::checkTaskPool()
{
	//qDebug() << "checking taskPool";
	printPool();
	auto it = taskPool.begin();
	while (it != taskPool.end()) {
		auto t     = *it;
		bool ready = sm.processDependencies(t->getDependencies());
		if (ready) {
			it = taskPool.erase(it);
			startTask(t);

			runningTasks[t->getId()] = t;
		} else {
			it++;
		}
	}
}

void TaskScheduler::startTask(std::shared_ptr<Task> task)
{
	qDebug() << "starting task" << task->getId();

	WorkerThread *thread = new WorkerThread(task);

	connect(thread, &WorkerThread::destroyed, this, &TaskScheduler::checkTaskPool,
	        Qt::QueuedConnection);

	connect(thread, &WorkerThread::taskFinished, this, &TaskScheduler::taskEnded,
	        Qt::QueuedConnection);

	thread->start();
}

void TaskScheduler::removeDuplicates(QString id)
{
	auto it = taskPool.begin();
	while (it != taskPool.end()) {
		auto    t = *it;
		QString taskDataId = t->getId();

		if (id == taskDataId) {
			t->invalidateSubscriptions();
			qDebug() << "task" << id << "gets removed from the task pool";
			it = taskPool.erase(it);
		} else {
			it++;
		}
	}

	{
		auto it = runningTasks.begin();
		while (it != runningTasks.end()) {
			auto    t = (*it).second;
			QString taskDataId = t->getId();

			if (id == taskDataId) {
				qDebug() << "cancelling execution of" << taskDataId << "task";
				t->cancel();
			}
			it++;
		}
	}
}

void TaskScheduler::removeDependantTasks(QString dataId)
{
	std::vector<QString> deps = sm.getDataDependencies(dataId);

	auto it = taskPool.begin();
	while (it != taskPool.end()) {
		auto    t = *it;
		QString targetDataId = t->targetId();

		auto depsIt = std::find(deps.begin(), deps.end(), targetDataId);
		if (depsIt != deps.end()) {
			auto id = t->getId();
			removeDependantTasks(id);
			t->invalidateSubscriptions();
			qDebug() << "task" << id << "gets removed from the task pool";
			it = taskPool.erase(it);
		} else {
			it++;
		}
	}
}

void TaskScheduler::taskEnded(QString id, bool success)
{
	if (!success) {
		qDebug() << "task" << id << "was aborted";
		std::shared_ptr<Task> t = runningTasks[id];

		if (t) {
			QString targetDataId = t->targetId();
			removeDependantTasks(targetDataId);
		}
	} else {
		qDebug() << "task" << id << "has finished";
	}

	runningTasks.erase(id);
	//checkTaskPool();
}

