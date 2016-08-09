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

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    int c;
    std::unique_ptr<Subscription> dataASub;
    std::unique_ptr<Subscription> dataBSub;
    std::unique_ptr<Subscription> dataCSub;
};

#endif // TASK_C_H
