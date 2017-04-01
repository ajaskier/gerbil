#include "model_d.h"
#include "task/task_d.h"
#include <data.h>
#include <QDebug>

#include <subscription_manager.h>
#include <task_scheduler.h>

ModelD::ModelD(int d, TaskScheduler* scheduler,
               QObject *parent)
	: Model(scheduler, parent), d(d)
{
	registerData("DATA_D", { "DATA_A" });
}

void ModelD::delegateTask(QString id, QString parentId)
{
	if (id == "DATA_D") {
		sendTask<TaskD>(d);
	}
}
