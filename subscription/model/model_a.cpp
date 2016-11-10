#include "model_a.h"
#include "task/task_a.h"
#include <task_scheduler.h>
#include <subscription_manager.h>
#include <data.h>
#include "lock.h"

#include "subscription_factory.h"
#include "subscription.h"

#include <QDebug>

ModelA::ModelA(int a, SubscriptionManager& sm, TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), a(a)
{
    registerData("DATA_A", {});
    //registerData("ROI", {});
}

void ModelA::delegateTask(QString id, QString parentId)
{
    if (id == "DATA_A") {
        //TaskA *task= new TaskA(a);
        std::shared_ptr<Task> task(new TaskA(a));
        sendTask(task);
        //scheduler->pushTask(task);
    } /*else if (id == "ROI") {

        calculateROI();
    }*/
}

void ModelA::calculateROI()
{
//    std::shared_ptr<Subscription> roiSub(
//                SubscriptionFactory::create(Dependency("ROI", SubscriptionType::WRITE),
//                                            SubscriberType::TASK));

//    Subscription::Lock<Data> lock(*roiSub);
//    Data d;
//    d.num = 4;

//    int version = lock.version();
//    lock.setVersion(version+1);
//    lock.swap(d);
}
