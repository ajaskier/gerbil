#include "model_a.h"
#include "task/task_a.h"
#include <task_scheduler.h>
#include <subscription_manager.h>
#include <data.h>
#include "lock.h"

#include <QDebug>

ModelA::ModelA(int a, TaskScheduler *scheduler, QObject *parent)
    : Model(scheduler, parent), a(a)
{
    registerData("DATA_A", {});
}

void ModelA::delegateTask(QString id, QString parentId)
{
    if (id == "DATA_A") {
        sendTask<TaskA>(a);
    }
}
