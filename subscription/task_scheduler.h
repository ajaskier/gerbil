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
    void removeRelated(QString id);
    void createSubscriptions(std::shared_ptr<Task> task);
    void taskEnded(QString id, bool success);

    void printPool();

    std::list<std::shared_ptr<Task>> taskPool;
    SubscriptionManager& sm;
};

#endif // TASK_SCHEDULER_H
