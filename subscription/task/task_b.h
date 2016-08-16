#ifndef TASK_B_H
#define TASK_B_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskB : public Task
{

public:
    explicit TaskB(int b);
    virtual ~TaskB();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    int b;
    std::shared_ptr<Subscription> dataASub;
    std::shared_ptr<Subscription> dataBSub;
};

#endif // TASK_B_H
