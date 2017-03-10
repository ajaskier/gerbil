#include "model_b.h"
#include "task/task_b.h"
#include "task/task_c.h"
#include <data.h>

#include <QDebug>

#include <subscription_manager.h>
#include <task_scheduler.h>

ModelB::ModelB(int b, int c, TaskScheduler* scheduler,
               QObject *parent)
    : Model(scheduler, parent), b(b), c(c)
{
    registerData("DATA_B", {"DATA_A"});
    registerData("DATA_C", {"DATA_B", "DATA_A"});
}

void ModelB::delegateTask(QString id, QString parentId)
{
    std::shared_ptr<Task> task;
    if (id == "DATA_B") {
        sendTask<TaskB>(b);
    } else if (id == "DATA_C") {
        sendTask<TaskC>(c);
    }
}
