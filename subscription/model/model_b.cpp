#include "model_b.h"
#include "task/task_b.h"
#include "task/task_c.h"
#include <data.h>

#include <QDebug>

#include <subscription_manager.h>
#include <task_scheduler.h>

ModelB::ModelB(int b, int c, SubscriptionManager &sm, TaskScheduler* scheduler,
               QObject *parent)
    : Model(sm, scheduler, parent), b(b), c(c)
{
    registerData("DATA_B", {"DATA_A"});
    registerData("DATA_C", {"DATA_A", "DATA_B"});
}

void ModelB::delegateTask(QString id)
{
    Task* task;
    if (id == "DATA_B") {
        task = new TaskB(b);
    } else if (id == "DATA_C") {
        task = new TaskC(c);
    }
    scheduler->pushTask(task);
}
