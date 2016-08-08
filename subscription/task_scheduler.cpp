#include "task_scheduler.h"
#include "subscription_manager.h"
#include "model/model.h"
#include "task/task.h"
#include "worker_thread.h"
#include "subscription.h"
#include "subscription_factory.h"
#include <QDebug>

TaskScheduler::TaskScheduler(SubscriptionManager &sm) : QObject(), sm(sm) {}

void TaskScheduler::pushTask(Task *task)
{
    qDebug() << "task" << task->getId() << "pushed to scheduler";
    removeRelated(task->getId());
    createSubscriptions(task);

    taskPool.push_front(task);
    checkTaskPool();
}

void TaskScheduler::createSubscriptions(Task *task)
{
    auto dependencies = task->getDependencies();
    for(auto& dependency : dependencies) {
        Subscription* s = SubscriptionFactory::create(dependency,
                                                      SubscriberType::TASK);
        task->setSubscription(dependency.dataId, s);
    }
}

void TaskScheduler::removeRelated(QString id)
{
    auto it = taskPool.begin();
    while(it != taskPool.end()) {

        auto dependencies = (*it)->getDependencies();
        if(std::any_of(dependencies.begin(), dependencies.end(),
                       [id](Dependency& dep){
                            return dep.dataId == id;
                        }))
        {
            qDebug() << "need to remove " << (*it)->getId();
            it = taskPool.erase(it);
        } else {
            ++it;
        }
    }
}

void TaskScheduler::checkTaskPool()
{
    qDebug() << "checking taskPool";
    auto it = taskPool.begin();
    while(it != taskPool.end()) {
        Task *t = *it;
        bool ready = sm.processDependencies(t->getDependencies());
        if(ready) {
            it = taskPool.erase(it);
            startTask(t);
        } else {
            it++;
        }
    }
}

void TaskScheduler::startTask(Task *task)
{
    qDebug() << "starting task" << task->getId();
    connect(task, SIGNAL(finished(Task*)), this, SLOT(taskFinished(Task*)),
            Qt::QueuedConnection);
    WorkerThread *thread = new WorkerThread(task);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void TaskScheduler::taskFinished(Task* task)
{
    qDebug() << "task" << task->getId() << "finished";
    task->endSubscriptions();
    task->deleteLater();
    checkTaskPool();
}
