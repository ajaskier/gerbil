#include "task_scheduler.h"
#include "subscription_manager.h"
#include "model/model.h"
#include <QDebug>

void TaskScheduler::appendTask(Task *task)
{

    auto signatures = task->getSignatures();
    for(auto& signature : signatures) {
        qDebug() << toString(signature.subscription);
        SubscriptionManager::instance().registerWill(task, signature.dataId,
                                                     signature.subscription);
    }

    qDebug() << "appending" << toString(task->getId()) << "task";
    taskPool.push_front(task);
    checkTaskPool();

}

void TaskScheduler::checkTaskPool()
{
    qDebug() << "checking task pool";

    for(Task* task : taskPool) {
        std::vector<DataId> dependencies = processSignature(task->getSignatures());
        if(dependencies.empty()) {
            qDebug() << "starting" << toString(task->getId()) << "task";
            connect(task, SIGNAL(finished(DataId)), this, SLOT(taskFinished(DataId)));
            task->start();
        } else {
            for(DataId data : dependencies) {
                SubscriptionManager::instance().askModelForTask(data);
            }
        }
    }
}

void TaskScheduler::taskFinished(DataId id)
{
    qDebug() << "task" << toString(id) << "finished";

    removeTask(id);

    qDebug() << "still" << taskPool.size() << "tasks in queue";
    checkTaskPool();
}

std::vector<DataId> TaskScheduler::processSignature(std::vector<Signature> &signatures)
{
    std::vector<DataId> dependencies;
    bool ready = true;
    for(Signature& sig : signatures) {
        DataId data = sig.dataId;
        if(sig.subscription == Subscription::WILL_READ) {
            ready = SubscriptionManager::instance().isInState(data, State::READ)
                    || SubscriptionManager::instance().isInState(data, State::VALID);
        } else if (sig.subscription == Subscription::WILL_WRITE) {
            ready = SubscriptionManager::instance().isInState(data, State::INVALID);
        }

        if(!ready) {
            bool inPool = isTaskInPool(data);
            if(!inPool) {
                dependencies.push_back(data);
                //schedule task for computation
            }
        }
    }
    return dependencies;
}

bool TaskScheduler::isTaskInPool(DataId id)
{
    for(Task* t : taskPool) {
        if(t->getId() == id) return true;
    }
    return false;
}

void TaskScheduler::removeTask(DataId id)
{
    for(auto it = taskPool.begin(); it != taskPool.end(); ) {
        if((*it)->getId() == id) {
           // (*it)->deleteLater();
            it = taskPool.erase(it);
        } else {
            ++it;
        }
    }
}
