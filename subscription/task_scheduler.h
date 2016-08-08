#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <QObject>

class Task;
class Dependency;
class SubscriptionManager;

class TaskScheduler : public QObject
{
    Q_OBJECT
public:
    TaskScheduler(SubscriptionManager& sm);
    void pushTask(Task* task);

protected slots:
    void taskFinished(Task *task);

protected:
    void checkTaskPool();
    void startTask(Task* task);
    void removeRelated(QString id);
    void createSubscriptions(Task* task);

    std::list<Task*> taskPool;
    SubscriptionManager& sm;
};

#endif // TASK_SCHEDULER_H
