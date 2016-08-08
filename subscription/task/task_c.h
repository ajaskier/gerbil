#ifndef TASK_C_H
#define TASK_C_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskC : public Task
{
    Q_OBJECT
public:
    explicit TaskC(int c, QObject* parent = 0);
    virtual ~TaskC();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    int c;
    Subscription* dataASub;
    Subscription* dataBSub;
    Subscription* dataCSub;
};

#endif // TASK_C_H
