#ifndef TASK_RESCALE_TBB_H
#define TASK_RESCALE_TBB_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>
#include <tbb/tbb.h>
#include "multi_img.h"

class TaskRescaleTbb : public Task
{

public:
    explicit TaskRescaleTbb(size_t bands, size_t roiBands, bool includecache = true);
    virtual ~TaskRescaleTbb();

    virtual bool run() override;

private:

    virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    size_t bands;
    size_t roiBands;
    size_t newsize;
    bool includecache;
    tbb::task_group_context stopper;

};

#endif // TASK_RESCALE_TBB_H
