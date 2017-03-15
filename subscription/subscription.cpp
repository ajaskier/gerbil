#include "subscription.h"
#include "subscription_manager.h"

int Subscription::spawnCounter = 0;
int Subscription::destroyCounter = 0;

Subscription::Subscription(Dependency dependency,
                           int id, SubscriptionManager &sm, QObject *requester,
                           std::function<void ()> updateSlot)
    : QObject(), dependency(dependency), /*accessType(accessType),*/
      sm(sm), id(id)
{
    spawnCounter++;
    //qDebug() << "created" << spawnCounter << "subs";

	if (requester && updateSlot)
		connect(this, &Subscription::update, requester, updateSlot,
            Qt::QueuedConnection);
    sm.subscribe(dependency.dataId, dependency.subscription, dependency.version, this);
}

Subscription::~Subscription()
{
    //qDebug() << "sub" << id << "deleted";

    if (!forcedDelete) {
        sm.unsubscribe(dependency.dataId, dependency.subscription, dependency.version, this);
    }
    {
        destroyCounter++;
        //qDebug() << "deleted" << destroyCounter << "subs";
    }
}

void Subscription::forceUnsubscribe()
{
    sm.unsubscribe(dependency.dataId, dependency.subscription, dependency.version, this, false);
    forcedDelete = true;
}

void Subscription::returnData()
{
    //qDebug() << dependency.dataId << toString(dependency.subscription) << "is returning";
    sm.endDoSubscription(dependency.dataId, dependency.subscription);
}

handle_pair Subscription::leaseData() {
    return sm.doSubscription(dependency.dataId, dependency.subscription);
}

int Subscription::version() {
    return sm.getMajorVersion(dependency.dataId);
}

void Subscription::setVersion(int version)
{
    sm.setMajorVersion(dependency.dataId, version);
}
