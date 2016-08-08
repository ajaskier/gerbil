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
    explicit TaskRescaleTbb(Subscription* imgSub, Subscription* imgIMGSub,
                            std::shared_ptr<multi_img> scoped, size_t bands, size_t roiBands,
                            bool includecache = true, QObject* parent = nullptr);
    virtual ~TaskRescaleTbb();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    Subscription* imgSub;
    Subscription* imgIMGSub;

    size_t bands;
    size_t roiBands;
    size_t newsize;
    bool includecache;
    tbb::task_group_context stopper;
    std::shared_ptr<multi_img> scoped;

};


#endif // TASK_RESCALE_TBB_H
