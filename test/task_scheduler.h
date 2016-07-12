#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <QObject>

class Task;
enum class DataId;
class Signature;

class TaskScheduler : public QObject
{
    Q_OBJECT
public:

    void appendTask(Task* task);

    static TaskScheduler& instance() {
        static TaskScheduler scheduler;
        return scheduler;
    }
    TaskScheduler(TaskScheduler const&) = delete;
    void operator=(TaskScheduler const&) = delete;

private slots:
    void taskFinished(DataId id);

private:
    TaskScheduler() : QObject() {}

    void checkTaskPool();
    std::vector<DataId> processSignature(std::vector<Signature>& signatures);
    bool isTaskInPool(DataId id);
    void removeTask(DataId id);

    std::list<Task*> taskPool;

};

#endif // TASK_SCHEDULER_H
