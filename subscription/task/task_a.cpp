#include "task_a.h"
#include "subscription_type.h"
#include "subscription.h"
#include <QThread>
#include "data.h"
#include <memory>
#include "lock.h"

TaskA::TaskA(int a, QObject* parent)
    : Task("DATA_A", parent), a(a)
{
    dependencies = {Dependency("DATA_A", SubscriptionType::WRITE)};
}

TaskA::~TaskA()
{
}

void TaskA::run()
{
    QThread::msleep(50);

    Data data;
    data.num = a;
    Subscription::Lock<Data> lock(*dataASub);
    qDebug() << "got dataA";
    lock.swap(data);

    qDebug() << "swapped";
}

void TaskA::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if(id == "DATA_A") dataASub = sub;
}
