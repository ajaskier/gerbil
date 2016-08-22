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

        std::shared_ptr<Subscription> s(
                    SubscriptionFactory::create(dependency,SubscriberType::TASK));
        task->setSubscription(dependency.dataId, std::move(s));
    }
}

void TaskScheduler::removeRelated(QString id)
{
    auto it = taskPool.begin();
    while(it != taskPool.end()) {

        auto dependencies = (*it)->getDependencies();
        if (std::any_of(dependencies.begin(), dependencies.end(),
                       [id](Dependency& dep){
                            return dep.dataId == id;
                        }))
        {
            qDebug() << "need to remove " << (*it)->getId();
            delete *it;
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
        if (ready) {
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

    WorkerThread *thread = new WorkerThread(task);
    connect(thread, &WorkerThread::finished, thread, &WorkerThread::deleteLater);
    connect(thread, &WorkerThread::finished, this, &TaskScheduler::checkTaskPool,
            Qt::QueuedConnection);

    thread->start();
}