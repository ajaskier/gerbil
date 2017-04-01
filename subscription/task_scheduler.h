#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <QObject>
#include <memory>

class Task;
class Dependency;
class SubscriptionManager;

class TaskScheduler : public QObject
{
	Q_OBJECT

public:
	TaskScheduler(SubscriptionManager& sm);
	void pushTask(std::shared_ptr<Task> task);

private:
	void checkTaskPool();
	void startTask(std::shared_ptr<Task> task);
	void createSubscriptions(std::shared_ptr<Task> task);
	void taskEnded(QString id, bool success);
	void removeDependantTasks(QString dataId);

	void printPool();

	std::list<std::shared_ptr<Task> > taskPool;

	std::map<QString, std::shared_ptr<Task>> runningTasks;

	SubscriptionManager& sm;
};

#endif // TASK_SCHEDULER_H
