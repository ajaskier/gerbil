#ifndef TASK_FAKE_SUB_H
#define TASK_FAKE_SUB_H

#include <QObject>
#include <task/task.h>

class TaskFakeSub : public Task
{

public:
    explicit TaskFakeSub(QString sourceId);
    virtual ~TaskFakeSub();

    virtual bool run() override;

private:

    virtual bool isCancelled() { return false; }

};


#endif // TASK_FAKE_SUB_H
