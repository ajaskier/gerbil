#ifndef TASK_B_H
#define TASK_B_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskB : public Task
{
    Q_OBJECT
public:
    explicit TaskB(int b, QObject* parent = 0);
    virtual ~TaskB();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    int b;
    Subscription* dataASub;
    Subscription* dataBSub;
};

#endif // TASK_B_H
