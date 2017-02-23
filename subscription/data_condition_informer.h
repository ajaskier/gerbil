#ifndef DATA_CONDITION_INFORMER_H
#define DATA_CONDITION_INFORMER_H

#include <QString>

class SubscriptionManager;

class DataConditionInformer
{

public:
    static void init(SubscriptionManager *subscriptionManager);
    static bool isInitialized(QString dataId);
    static bool isUpToDate(QString dataId);

    static int majorVersion(QString dataId);
    static int minorVersion(QString dataId);
private:

    static SubscriptionManager* sm;
};

#endif // DATA_CONDITION_INFORMER_H
