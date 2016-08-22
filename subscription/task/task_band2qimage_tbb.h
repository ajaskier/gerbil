#ifndef TASK_BAND2QIMAGE_TBB_H
#define TASK_BAND2QIMAGE_TBB_H

#include <task/task.h>

#include <tbb/blocked_range2d.h>
#include <tbb/task_group.h>

class TaskBand2QImageTbb : public Task
{

public:
    explicit TaskBand2QImageTbb(QString sourceId, QString destId, size_t dim);
    virtual ~TaskBand2QImageTbb();

    virtual bool run() override;

private:

    virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    size_t dim;
    tbb::task_group_context stopper;

};


#endif // TASK_BAND2QIMAGE_TBB_H
