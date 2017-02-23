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

bool DataConditionInformer::isUpToDate(QString dataId)
{
    return sm->isUpToDate(dataId);
}

int DataConditionInformer::majorVersion(QString dataId)
{
    return sm->getMajorVersion(dataId);
}

int DataConditionInformer::minorVersion(QString dataId)
{
    return sm->getMinorVersion(dataId);
}
