#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <QString>
#include "subscription_type.h"

enum class AccessType;

class Dependency {

public:
    Dependency(QString dataId, SubscriptionType subscription, AccessType accessType,
               int version = -1)
        : dataId(dataId), subscription(subscription),
          accessType(accessType), version(version) {}

    QString dataId;
    SubscriptionType subscription;
	AccessType accessType;
	int version;
};


#endif // DEPENDENCY_H
