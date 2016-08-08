#ifndef TASK_A_H
#define TASK_A_H

#include <QObject>
#include <task/task.h>

class TaskA : public Task
{
    Q_OBJECT
public:
    explicit TaskA(int a, QObject* parent = 0);
    virtual ~TaskA();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    int a;
    Subscription* dataASub;

};

#endif // TASK_A_H
