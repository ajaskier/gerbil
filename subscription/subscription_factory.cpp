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
                                          AccessType accessType)
{
    assert(sm);
    return new Subscription(dependency, accessType, idCounter++, *sm);
}

Subscription* SubscriptionFactory::create(Dependency dependency,
                                          AccessType accessType,
                                          QObject *requester,
                                          std::function<void ()> updateSlot)
{
    assert(sm);
    return new Subscription(dependency, accessType, idCounter++, *sm,
                            requester, updateSlot);
}
