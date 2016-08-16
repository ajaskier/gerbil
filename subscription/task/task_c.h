#ifndef TASK_C_H
#define TASK_C_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskC : public Task
{

public:
    explicit TaskC(int c);
    virtual ~TaskC();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    int c;
    std::shared_ptr<Subscription> dataASub;
    std::shared_ptr<Subscription> dataBSub;
    std::shared_ptr<Subscription> dataCSub;
};

#endif // TASK_C_H
