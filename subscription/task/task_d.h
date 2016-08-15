#ifndef TASK_D_H
#define TASK_D_H

#include <QObject>
#include <task/task.h>
#include "lock.h"

class TaskD : public Task
{
    Q_OBJECT
public:
    explicit TaskD(int d, QObject* parent = 0);
    virtual ~TaskD();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    int d;

    std::shared_ptr<Subscription> dataASub;
    std::shared_ptr<Subscription> dataDSub;
};

#endif // TASK_D_H
