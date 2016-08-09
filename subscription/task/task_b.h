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

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    int b;
    std::unique_ptr<Subscription> dataASub;
    std::unique_ptr<Subscription> dataBSub;
};

#endif // TASK_B_H
