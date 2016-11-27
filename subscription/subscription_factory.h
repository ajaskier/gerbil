#ifndef SUBSCRIPTION_FACTORY_H
#define SUBSCRIPTION_FACTORY_H

#include <QObject>
#include <functional>

enum class AccessType;
class SubscriptionManager;
class Dependency;
class Subscription;

class SubscriptionFactory
{

public:
    static void init(SubscriptionManager *subscriptionManager);
    static Subscription* create(Dependency dependency, AccessType accessType);
    static Subscription* create(Dependency dependency, AccessType accessType,
                                QObject *requester, std::function<void(void)> updateSlot);

private:

    static SubscriptionManager* sm;
    static int idCounter;
};

#endif // SUBSCRIPTION_FACTORY_H
