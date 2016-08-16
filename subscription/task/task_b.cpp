#include "task_b.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskB::TaskB(int b, QObject *parent)
    : Task("DATA_B", parent), b(b)
{
    dependencies = { Dependency("DATA_B", SubscriptionType::WRITE),
                   Dependency("DATA_A", SubscriptionType::READ) };
}

TaskB::~TaskB()
{
}

void TaskB::run()
{
    Data data;
    //read phase
    {
        Subscription::Lock<Data> lock(*dataASub);
        data.num = b+lock()->num;
        //QThread::sleep(5);
    }
    //write phase
    {
        Subscription::Lock<Data> lock(*dataBSub);
        lock.swap(data);
    }

}

void TaskB::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if (id == "DATA_A") dataASub = sub;
    else if (id == "DATA_B") dataBSub = sub;
}
