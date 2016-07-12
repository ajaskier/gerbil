#include "model.h"
#include <QDebug>
#include "subscription_manager.h"

void Model::registerData(DataId dataId) {
    SubscriptionManager::instance().registerCreator(this, dataId);
}
