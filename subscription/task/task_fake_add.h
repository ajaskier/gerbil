#ifndef TASK_FAKE_ADD_H
#define TASK_FAKE_ADD_H

#include <QObject>
#include <task/task.h>

class TaskFakeAdd : public Task
{

public:
    explicit TaskFakeAdd(QString destId, QString imageId);
    explicit TaskFakeAdd(QString destId, QString imageId, QString tempId);
    virtual ~TaskFakeAdd();

    virtual bool run() override;

private:

    virtual bool isCancelled() { return false; }

};


#endif // TASK_FAKE_ADD_H
