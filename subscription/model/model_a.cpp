#include "model_a.h"
#include "task/task_a.h"
#include <task_scheduler.h>
#include <subscription_manager.h>
#include <data.h>
#include "lock.h"

#include "subscription_factory.h"
#include "subscription.h"

#include <QDebug>

ModelA::ModelA(int a, SubscriptionManager& sm, TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), a(a)
{
    registerData("DATA_A", {});
}

void ModelA::delegateTask(QString id, QString parentId)
{
    if (id == "DATA_A") {
        sendTask(std::make_shared<TaskA>(a));
    }
}
