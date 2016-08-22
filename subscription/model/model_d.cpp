#include "model_d.h"
#include "task/task_d.h"
#include <data.h>
#include <QDebug>

#include <subscription_manager.h>
#include <task_scheduler.h>

ModelD::ModelD(int d, SubscriptionManager &sm, TaskScheduler* scheduler,
               QObject *parent)
    : Model(sm, scheduler, parent), d(d)
{
    registerData("DATA_D", {"DATA_A"});
}

void ModelD::delegateTask(QString id)
{
    if (id == "DATA_D") {
        Task* task = new TaskD(d);
        scheduler->pushTask(task);
    }
}