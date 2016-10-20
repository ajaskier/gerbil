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

    void stop();
    void resume();

protected:
    void checkTaskPool();
    void startTask(Task* task);
    void removeRelated(QString id);
    void createSubscriptions(Task* task);

    std::list<Task*> taskPool;
    SubscriptionManager& sm;

    int stopped = 0;
};

#endif // TASK_SCHEDULER_H
