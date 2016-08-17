#ifndef TASK_NORML2TBB_H
#define TASK_NORML2TBB_H

#include <QObject>
#include <task/task.h>

#include <tbb/tbb.h>

class TaskNormL2Tbb : public Task
{

public:
    explicit TaskNormL2Tbb();
    virtual ~TaskNormL2Tbb();

    virtual bool run() override;

private:

    tbb::task_group_context stopper;
};

#endif // TASK_NORML2TBB_H
