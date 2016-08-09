#include "subscription.h"
#include "subscription_manager.h"

int Subscription::spawnCounter = 0;
int Subscription::destroyCounter = 0;

Subscription::Subscription(Dependency dependency,  SubscriberType subscriberType,
                           int id, SubscriptionManager& sm) :
    QObject(), dependency(dependency), subscriberType(subscriberType), sm(sm),
    id(id)
{
    {
        spawnCounter++;
        qDebug() << "created" << spawnCounter << "subs";
    }


    sm.subscribe(dependency.dataId, dependency.subscription, this);
}

Subscription::Subscription(Dependency dependency, SubscriberType subscriberType,
                           int id, SubscriptionManager &sm, QObject *requester,
                           std::function<void ()> updateSlot)
    : QObject(), dependency(dependency), subscriberType(subscriberType), sm(sm),
      id(id)
{
    {
        spawnCounter++;
        qDebug() << "created" << spawnCounter << "subs";
    }

    connect(this, &Subscription::update, requester, updateSlot,
            Qt::QueuedConnection);
    sm.subscribe(dependency.dataId, dependency.subscription, this);
}

Subscription::~Subscription()
{
    //qDebug() << "sub" << id << "deleted";
    sm.unsubscribe(dependency.dataId, dependency.subscription, this);
    {
        destroyCounter++;
        qDebug() << "deleted" << destroyCounter << "subs";
    }
}

void Subscription::returnData()
{
    //qDebug() << dependency.dataId << toString(dependency.subscription) << "is returning";
    sm.endDoSubscription(dependency.dataId, dependency.subscription);
}

void Subscription::forceUpdate()
{
    emit update();
}

any_sptr Subscription::leaseData() {
    return sm.doSubscription(dependency.dataId, dependency.subscription);
}
