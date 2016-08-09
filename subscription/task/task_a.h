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

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    int a;
    std::unique_ptr<Subscription> dataASub;

};

#endif // TASK_A_H
