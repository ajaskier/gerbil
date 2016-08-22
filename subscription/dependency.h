#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <QString>
#include "subscription_type.h"

class Dependency {

public:
    Dependency(QString dataId, SubscriptionType subscription)
        : dataId(dataId), subscription(subscription) {}

    QString dataId;
    SubscriptionType subscription;
};


#endif // DEPENDENCY_H
