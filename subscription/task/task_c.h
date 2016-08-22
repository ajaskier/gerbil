#ifndef TASK_C_H
#define TASK_C_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskC : public Task
{

public:
    explicit TaskC(int c);
    virtual ~TaskC();

    virtual bool run() override;

private:

    int c;
};

#endif // TASK_C_H
