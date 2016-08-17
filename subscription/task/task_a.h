#ifndef TASK_A_H
#define TASK_A_H

#include <QObject>
#include <task/task.h>

class TaskA : public Task
{

public:
    explicit TaskA(int a);
    virtual ~TaskA();

    virtual bool run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    int a;
    std::shared_ptr<Subscription> dataASub;

};

#endif // TASK_A_H
