#ifndef TASK_GRADIENT_TBB_H
#define TASK_GRADIENT_TBB_H

#include <QObject>
#include <task/task.h>
#include <tbb/tbb.h>

#include <opencv2/core/core.hpp>

class TaskGradientTbb : public Task
{

public:
    explicit TaskGradientTbb(bool includecache = true);
    virtual ~TaskGradientTbb();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    std::shared_ptr<Subscription> sourceSub;
    std::shared_ptr<Subscription> currentSub;
    tbb::task_group_context stopper;
    bool includecache;
};


#endif // TASK_GRADIENT_TBB_H
