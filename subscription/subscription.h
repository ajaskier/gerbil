#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <QObject>
#include <memory>
#include <functional>
#include <QDebug>

#include <boost/any.hpp>
#include <subscription_manager.h>
#include "dependency.h"

enum class SubscriberType {
    TASK,
    READER
};

class Subscription : public QObject
{
    Q_OBJECT
public:

    virtual ~Subscription() override;
    void returnData();
    void forceUpdate();
    int getId() { return id; }
    SubscriberType getSubscriberType() { return subscriberType; }

    template <class T>
    class Lock;

signals:
    void update();

private:
    explicit Subscription(Dependency dependency, SubscriberType subscriberType,
                          int id, SubscriptionManager& sm);

    explicit Subscription(Dependency dependency, SubscriberType subscriberType,
                          int id, SubscriptionManager& sm, QObject* requester,
                          std::function<void(void)> updateSlot);
    any_sptr leaseData();

    Dependency dependency;
    SubscriberType subscriberType;
    SubscriptionManager& sm;
    const int id;
    static int spawnCounter;
    static int destroyCounter;

    bool ended = false;
    friend class SubscriptionFactory;
};

#endif // SUBSCRIPTION_H
