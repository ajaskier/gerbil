#ifndef DATA_CONDITION_INFORMER_H
#define DATA_CONDITION_INFORMER_H

#include <QString>

class SubscriptionManager;

class DataConditionInformer
{

public:
    static void init(SubscriptionManager *subscriptionManager);
    static bool isInitialized(QString dataId);

private:

    static SubscriptionManager* sm;
};

#endif // DATA_CONDITION_INFORMER_H
