#include "task_c.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"

TaskC::TaskC(int c, QObject *parent)
    : Task("DATA_C", parent), c(c)
{
    dependencies = { Dependency("DATA_C", SubscriptionType::WRITE),
                   Dependency("DATA_B", SubscriptionType::READ),
                   Dependency("DATA_A", SubscriptionType::READ) };
}

TaskC::~TaskC()
{
}

void TaskC::run()
{

    Data data;
    {
        Subscription::Lock<Data> lockA(*dataASub);
        Subscription::Lock<Data> lockB(*dataBSub);
        //QThread::sleep(3);
        data.num = c + lockA()->num + lockB()->num;
    }

    Subscription::Lock<Data> lockC(*dataCSub);
    lockC.swap(data);

}

void TaskC::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if (id == "DATA_A") dataASub = sub;
    else if (id == "DATA_B") dataBSub = sub;
    else if (id == "DATA_C") dataCSub = sub;
}
