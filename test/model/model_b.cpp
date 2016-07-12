#include "model_b.h"
#include <QDebug>

#include <subscription_manager.h>
#include <task_scheduler.h>

TaskB::TaskB(int b, QObject *parent)
    : Task(DataId::DATA_B, parent), b(b)
{
    signatures = { Signature(DataId::DATA_B, Subscription::WILL_WRITE),
                   Signature(DataId::DATA_A, Subscription::WILL_READ) };
}

void TaskB::run()
{

    std::shared_ptr<MultiType> data_a = SubscriptionManager::instance().doSubscription(
                DataId::DATA_A, Subscription::DO_READ);
    auto dataA = std::static_pointer_cast<SpecificType<Data>>(data_a);

    int calculation = -1*b + (*dataA)().num;

    SubscriptionManager::instance().endDoSubscription(DataId::DATA_A,
                                                      Subscription::DO_READ);

    std::shared_ptr<MultiType> data_b = SubscriptionManager::instance().doSubscription(
                DataId::DATA_B, Subscription::DO_WRITE);
    auto dataB = std::static_pointer_cast<SpecificType<Data>>(data_b);

    (*dataB)().num = calculation;

    SubscriptionManager::instance().endDoSubscription(DataId::DATA_B,
                                                      Subscription::DO_WRITE);

    //emit finished(id);

}

ModelB::ModelB(int b, QObject *parent)
    : Model(parent), b(b)
{
    registerData(DataId::DATA_B);
}

void ModelB::delegateTask()
{
    TaskB *task = new TaskB(3);
    TaskScheduler::instance().appendTask(task);
}
