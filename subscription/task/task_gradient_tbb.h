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

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    std::unique_ptr<Subscription> imgIMGSub;
    std::unique_ptr<Subscription> imgGRADSub;
    bool includecache;
};


#endif // TASK_GRADIENT_TBB_H
