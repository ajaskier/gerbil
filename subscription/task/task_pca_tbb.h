#ifndef TASK_PCA_TBB_H
#define TASK_PCA_TBB_H

#include <QObject>
#include <task/task.h>
#include <tbb/tbb.h>

#include <opencv2/core/core.hpp>

class TaskPcaTbb : public Task
{

public:
    explicit TaskPcaTbb(unsigned int components = 0, bool includecache = true);
    virtual ~TaskPcaTbb();

    virtual bool run() override;

private:

    virtual bool isCancelled() { return stopper.is_group_execution_cancelled(); }

    tbb::task_group_context stopper;
    unsigned int components;
    bool includecache;
};


#endif // TASK_PCA_TBB_H
