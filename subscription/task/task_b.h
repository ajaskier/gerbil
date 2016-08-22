#ifndef TASK_B_H
#define TASK_B_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskB : public Task
{

public:
    explicit TaskB(int b);
    virtual ~TaskB();

    virtual bool run() override;

private:

    int b;
};

#endif // TASK_B_H
