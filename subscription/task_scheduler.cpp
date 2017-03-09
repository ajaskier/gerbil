#include "task_scheduler.h"
#include "subscription_manager.h"
#include "model/model.h"
#include "task/task.h"
#include "worker_thread.h"
#include "subscription.h"
#include "data_register.h"
#include <QDebug>

TaskScheduler::TaskScheduler(SubscriptionManager &sm) : QObject(), sm(sm) {}

void TaskScheduler::pushTask(std::shared_ptr<Task> task)
{

    createSubscriptions(task);

    taskPool.push_front(task);
    checkTaskPool();

}

void TaskScheduler::printPool()
{
    for(auto task : taskPool) qDebug() << "in pool: " << task->getId();
}

void TaskScheduler::createSubscriptions(std::shared_ptr<Task> task)
{
    auto dependencies = task->getDependencies();
    for(auto& dependency : dependencies) {

        std::shared_ptr<Subscription> s(
                    DataRegister::subscribe(dependency));
        task->setSubscription(dependency.dataId, std::move(s));
    }
}

void TaskScheduler::checkTaskPool()
{

    qDebug() << "checking taskPool";
    printPool();
    auto it = taskPool.begin();
    while(it != taskPool.end()) {
        auto t = *it;
        bool ready = sm.processDependencies(t->getDependencies());
        if (ready) {
            it = taskPool.erase(it);
            startTask(t);
        } else {
            it++;
        }
    }
}

void TaskScheduler::startTask(std::shared_ptr<Task> task)
{
    qDebug() << "starting task" << task->getId();

    WorkerThread *thread = new WorkerThread(task);

    connect(thread, &WorkerThread::destroyed, this, &TaskScheduler::checkTaskPool,
            Qt::QueuedConnection);


    thread->start();
}

