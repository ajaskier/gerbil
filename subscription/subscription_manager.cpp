#include "subscription_manager.h"

#include "model/model.h"
#include "task/task.h"
#include "task_scheduler.h"
#include "subscription.h"
#include "dependency.h"

#include <QDebug>

void SubscriptionManager::registerCreator(Model* creator, QString dataId,
                                          std::vector<QString> dependencies) {

    assert(dataPool.find(dataId) == dataPool.end());

    dataPool[dataId].handle = std::shared_ptr<boost::dynamic_any>(
                new boost::dynamic_any());
    dataPool[dataId].creator = creator;

    for(QString& s : dependencies) {
        dataPool[s].dependants.push_back(dataId);
    }
}

void SubscriptionManager::subscribe(QString dataId, SubscriptionType sub,
                                    Subscription* subObj)
{
    dataPool[dataId].currentSubs[subObj->getId()] = subObj;

    if(sub == SubscriptionType::READ) subscribeRead(dataId, subObj);
    else subscribeWrite(dataId);

    updateState(dataId);
}

void SubscriptionManager::subscribeRead(QString dataId, Subscription* subObj)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    bool instantRequest = subObj->getSubscriberType() == SubscriberType::READER;
    dataPool[dataId].willReads++;
    if(dataPool[dataId].validity == ValidityState::INVALID && !dataPool[dataId].willWrite) {
        askModelForTask(dataId);
    } else if(dataPool[dataId].access != AccessState::WRITE && instantRequest
              && dataPool[dataId].initialized) {
        sendUpdate(dataId);
    }
}

void SubscriptionManager::subscribeWrite(QString dataId)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[dataId].willWrite = true;
    dataPool[dataId].upToDate = false;
    updateState(dataId);
    invalidDependants(dataId);
}

void SubscriptionManager::unsubscribe(QString dataId, SubscriptionType sub,
                                      Subscription *subObj)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    dataPool[dataId].currentSubs.erase(subObj->getId());
    if(sub == SubscriptionType::READ) dataPool[dataId].willReads--;
    else dataPool[dataId].willWrite = false;
}

any_sptr SubscriptionManager::doSubscription(QString id, SubscriptionType sub) {
    if(sub == SubscriptionType::READ) return doReadSubscription(id);
    else return doWriteSubscription(id);
}

any_sptr SubscriptionManager::doReadSubscription(QString id)
{
    any_sptr data = dataPool[id].read();

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doReads++;
    updateState(id);

    return data;
}

any_sptr SubscriptionManager::doWriteSubscription(QString id)
{
    any_sptr data = dataPool[id].write();

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].upToDate = false;
    dataPool[id].doWrite = true;

    updateState(id);
    return data;
}

void SubscriptionManager::endDoSubscription(QString id, SubscriptionType sub)
{
    if(sub == SubscriptionType::READ) endDoReadSubscription(id);
    else endDoWriteSubscription(id);
}

void SubscriptionManager::endDoReadSubscription(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doReads--;
    dataPool[id].endRead();
    updateState(id);
}

void SubscriptionManager::endDoWriteSubscription(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doWrite = false;
    dataPool[id].upToDate = true;
    dataPool[id].initialized = true;
    dataPool[id].endWrite();

    updateState(id);
    sendUpdate(id);
    propagateChange(id);
}

void SubscriptionManager::propagateChange(QString id) {
    for(QString& data: dataPool[id].dependants) {
        if(hasWillReads(data) && !dataPool[data].willWrite) {
            askModelForTask(data);
        }
        propagateChange(data);
    }
}

void SubscriptionManager::invalidDependants(QString id)
{
    for(QString& data: dataPool[id].dependants) {
        dataPool[data].upToDate = false;
        updateState(data);
        invalidDependants(data);
    }
}

void SubscriptionManager::updateState(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    if(dataPool[id].upToDate) dataPool[id].validity = ValidityState::VALID;
    else dataPool[id].validity = ValidityState::INVALID;

    if(!dataPool[id].doWrite && dataPool[id].doReads == 0)
    {
        dataPool[id].access = AccessState::NONE;
    }
    else if(dataPool[id].doWrite && dataPool[id].doReads == 0)
    {
        dataPool[id].access = AccessState::WRITE;
    } else if(!dataPool[id].doWrite && dataPool[id].doReads > 0)
    {
        dataPool[id].access = AccessState::READ;
    } else {
        assert(false);
    }
}

void SubscriptionManager::sendUpdate(QString id)
{
    auto subs = dataPool[id].currentSubs;
    for(auto sub : subs) {
        sub.second->update();
    }
}

bool SubscriptionManager::hasWillReads(QString parentId) {

    if(dataPool[parentId].willReads > 0)
        return true;

    auto& deps = dataPool[parentId].dependants;

    return std::any_of(deps.begin(), deps.end(), [this](QString& data) {
        return dataPool[data].willReads > 0 || hasWillReads(data);
    });
}

void SubscriptionManager::askModelForTask(QString id)
{
    qDebug() << "asking for" << id;
    dataPool[id].creator->delegateTask(id);
}

bool SubscriptionManager::processDependencies(std::vector<Dependency> &dependencies)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    for(Dependency& dep : dependencies) {
        QString dataId = dep.dataId;

        if(dep.subscription == SubscriptionType::READ
                && (dataPool[dataId].access == AccessState::WRITE
                    || dataPool[dataId].validity != ValidityState::VALID))
            return false;

        if (dep.subscription == SubscriptionType::WRITE
                && dataPool[dataId].access != AccessState::NONE)
            return false;
    }
    return true;
}
