#ifndef TASK_GRADIENT_TBB_H
#define TASK_GRADIENT_TBB_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>

class TaskGradientTbb : public Task
{
    Q_OBJECT
public:
    explicit TaskGradientTbb(bool includecache = true, QObject* parent = nullptr);
    virtual ~TaskGradientTbb();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    Subscription* imgIMGSub;
    Subscription* imgGRADSub;
    bool includecache;
};


#endif // TASK_GRADIENT_TBB_H
