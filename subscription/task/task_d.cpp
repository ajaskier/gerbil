#include "task_d.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskD::TaskD(int d)
    : Task("DATA_D"), d(d)
{
    dependencies = { Dependency("DATA_D", SubscriptionType::WRITE),
                   Dependency("DATA_A", SubscriptionType::READ) };
}

TaskD::~TaskD()
{
}

void TaskD::run()
{

    Data data;
    {
        Subscription::Lock<Data> lockA(*dataASub);
        data.num = d + lockA()->num + 1;
        //QThread::sleep(3);
    }

    Subscription::Lock<Data> lockD(*dataDSub);
    lockD.swap(data);
    lockD.release();

}

void TaskD::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if (id == "DATA_A") dataASub = sub;
    else if (id == "DATA_D") dataDSub = sub;
}
