#include "model.h"
#include "task_scheduler.h"
#include <QDebug>

Model::Model(SubscriptionManager &sm, TaskScheduler *scheduler, QObject *parent)
    : QObject(parent), sm(sm), scheduler(scheduler)
{

}

Model::~Model()
{

}

void Model::registerData(QString dataId, std::vector<QString> dependencies)
{
    sm.registerCreator(this, dataId, dependencies);
}

void Model::sendTask(std::shared_ptr<Task> t)
{
    QString id = t->getId();
    if (tasks.find(id) != tasks.end()) {
        qDebug() << "I'm not sending the task" << id;
        return;
    }

    QObject::connect(t.get(), &Task::finished, this, &Model::taskFinished);
    tasks[t->getId()] = t;
    scheduler->pushTask(t);
}

void Model::taskFinished(QString id, bool success)
{
    qDebug() << "removing the task in model";
    tasks.erase(id);
}
