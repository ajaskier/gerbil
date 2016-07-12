#include "model_a.h"
#include <task_scheduler.h>
#include <subscription_manager.h>

#include <QDebug>


TaskA::TaskA(int a, QObject* parent)
    : Task(DataId::DATA_A, parent), a(a)
{
    signatures.push_back(Signature(DataId::DATA_A,
                        Subscription::WILL_WRITE));
}

void TaskA::run() {

    int calculation = a*a;

    //get data
    std::shared_ptr<MultiType> data = SubscriptionManager::instance().doSubscription(
                DataId::DATA_A, Subscription::DO_WRITE);
    auto dataA = std::static_pointer_cast<SpecificType<Data>>(data);
    (*dataA)().num = calculation;

    SubscriptionManager::instance().endDoSubscription(DataId::DATA_A,
                                                      Subscription::DO_WRITE);

    //emit finished(id);
}

ModelA::ModelA(int a, QObject *parent)
    : Model(parent), a(a)
{
    registerData(DataId::DATA_A);
}

void ModelA::delegateTask()
{
    TaskA *task= new TaskA(5);
    TaskScheduler::instance().appendTask(task);
}

