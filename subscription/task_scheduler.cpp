#include "task_scheduler.h"
#include "subscription_manager.h"
#include "model/model.h"
#include "task/task.h"
#include "worker_thread.h"
#include "subscription.h"
#include "subscription_factory.h"
#include <QDebug>

TaskScheduler::TaskScheduler(SubscriptionManager &sm) : QObject(), sm(sm) {}

void TaskScheduler::pushTask(std::shared_ptr<Task> task)
{
    //qDebug() << "task" << task->getId() << "landed in scheduler";
//    if (runningTasks[task->getId()]) {
//        qDebug() << "there's already task running";
//        //delete task;
//        return;
//    }

    //removeRelated(task->getId());
    //removeRelated(task->getDependencies());
    createSubscriptions(task);


    //removeRelated(task->getId()); //DIRTY HACK
    //qDebug() << "task" << task->getId() << "going to the pool";
    taskPool.push_front(task);
    checkTaskPool();
    //if (!stopped) checkTaskPool();
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
                    SubscriptionFactory::create(dependency,AccessType::DIRECT));
        task->setSubscription(dependency.dataId, std::move(s));
    }
}


// this method is no good with task passed as shared_ptr
void TaskScheduler::removeRelated(QString id)
{
//    auto it = taskPool.begin();
//    while(it != taskPool.end()) {

//        auto comparedId = (*it)->getId();
////        auto dependencies = (*it)->getDependencies();
////        if (std::any_of(dependencies.begin(), dependencies.end(),
////                       [id](Dependency& dep){
////                            return dep.dataId == id;
////                        }) || comparedId == id)
//        if (comparedId == id)
//        {
//            qDebug() << "need to remove " << (*it)->getId();
//            (*it)->invalidateSubscriptions();
//            delete *it;
//            it = taskPool.erase(it);
//        } else {
//            ++it;
//        }
//    }
}

void TaskScheduler::checkTaskPool()
{
//    if (stopped)
//        return;

    qDebug() << "checking taskPool";
    printPool();
    auto it = taskPool.begin();
    while(it != taskPool.end()) {
        auto t = *it;
        //qDebug() << "checking task" << t->getId();
        if (t->getId() == "taskAdd"/* || t->getId() == "image.IMG"*/) {
            qDebug() << "finally";
        }
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
    //connect(thread, &WorkerThread::finished, thread, &WorkerThread::deleteLater);
//    connect(thread, &WorkerThread::destroyed, this, &TaskScheduler::taskEnded,
//            Qt::QueuedConnection);

    connect(thread, &WorkerThread::destroyed, this, &TaskScheduler::checkTaskPool,
            Qt::QueuedConnection);


    thread->start();
}

//void TaskScheduler::taskEnded(QString id, bool success)
//{
//    runningTasks[id] = false;
//    qDebug() << id << "task finished";
//    qDebug() << taskPool.size() << "tasks in queue";
//}

//void TaskScheduler::stop()
//{
//    qDebug() << "scheduler stopped";
//    stopped++;
//}

//void TaskScheduler::resume()
//{
//    stopped--;
//    if(!stopped) qDebug() << "scheduler resumed";
//    if(!stopped) checkTaskPool();
//}
