#include "model.h"
#include "task_scheduler.h"
#include "data_register.h"
#include <QDebug>

Model::Model(TaskScheduler *scheduler, QObject *parent)
	: QObject(parent), scheduler(scheduler)
{}

Model::~Model()
{}

void Model::registerData(QString dataId, std::vector<QString> dependencies)
{
	DataRegister::add(this, dataId, dependencies);
}

bool Model::isTaskCurrent(QString id)
{
	return tasks.find(id) != tasks.end();
}

void Model::sendTask(std::shared_ptr<Task> t)
{
//	QString id = t->getId();
//	if (isTaskCurrent(id)) {
//		//qDebug() << "Task is current, new task will not be sent" << id;
//		return;
//	}

	QObject::connect(t.get(), &Task::taskFinished, this, &Model::taskFinished);
//	tasks[t->getId()] = t;
	scheduler->pushTask(t);
}

void Model::taskFinished(QString id, bool success)
{
	//qDebug() << "removing the task" << id << "in model";
	//tasks.erase(id);
}
