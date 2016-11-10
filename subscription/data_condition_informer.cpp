#include "data_condition_informer.h"

#include "subscription_manager.h"
#include <QDebug>

SubscriptionManager* DataConditionInformer::sm = nullptr;

void DataConditionInformer::init(SubscriptionManager* subscriptionManager) {
    sm = subscriptionManager;
}

bool DataConditionInformer::isInitialized(QString dataId)
{
    return sm->isDataInitialized(dataId);
}

int DataConditionInformer::version(QString dataId)
{
    return sm->getVersion(dataId);
}
