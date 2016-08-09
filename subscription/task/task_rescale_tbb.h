#ifndef TASK_RESCALE_TBB_H
#define TASK_RESCALE_TBB_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>
#include <tbb/tbb.h>
#include "multi_img.h"

class TaskRescaleTbb : public Task
{
    Q_OBJECT
public:
    explicit TaskRescaleTbb(size_t bands, size_t roiBands,
                            bool includecache = true, QObject* parent = nullptr);
    virtual ~TaskRescaleTbb();

    virtual void run() override;

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    std::unique_ptr<Subscription> sourceSub;
    std::unique_ptr<Subscription> currentSub;

    size_t bands;
    size_t roiBands;
    size_t newsize;
    bool includecache;
    tbb::task_group_context stopper;

};

#endif // TASK_RESCALE_TBB_H
