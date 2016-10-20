#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <QString>
#include "subscription_type.h"

class Dependency {

public:
    Dependency(QString dataId, SubscriptionType subscription, int version = -1)
        : dataId(dataId), subscription(subscription), version(version) {}

    QString dataId;
    int version;
    SubscriptionType subscription;

};


#endif // DEPENDENCY_H
