#include "subscription_factory.h"

#include "subscription_manager.h"
#include "dependency.h"
#include "subscription.h"
#include <QDebug>

int SubscriptionFactory::idCounter = 0;
SubscriptionManager* SubscriptionFactory::sm = nullptr;

void SubscriptionFactory::init(SubscriptionManager* subscriptionManager) {
    sm = subscriptionManager;
}

Subscription* SubscriptionFactory::create(Dependency dependency,
                                          SubscriberType subscriberType)
{
    assert(sm);
    return new Subscription(dependency, subscriberType, idCounter++, *sm);
}

Subscription* SubscriptionFactory::create(Dependency dependency,
                                          SubscriberType subscriberType,
                                          QObject *requester,
                                          std::function<void ()> updateSlot)
{
    assert(sm);
    return new Subscription(dependency, subscriberType, idCounter++, *sm,
                            requester, updateSlot);
}
