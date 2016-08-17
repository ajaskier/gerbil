#ifndef TASK_GRADIENT_TBB_H
#define TASK_GRADIENT_TBB_H

#include <task/task.h>
#include <tbb/tbb.h>

#include <opencv2/core/core.hpp>

class TaskGradientTbb : public Task
{

public:
    explicit TaskGradientTbb(bool includecache = true);
    virtual ~TaskGradientTbb();

    virtual bool run() override;
private:

    tbb::task_group_context stopper;
    bool includecache;
};


#endif // TASK_GRADIENT_TBB_H
