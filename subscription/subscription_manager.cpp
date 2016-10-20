#include "subscription_manager.h"

#include "model/model.h"
#include "task/task.h"
#include "task_scheduler.h"
#include "subscription.h"
#include "dependency.h"

#include <QDebug>

SubscriptionManager::SubscriptionManager(QObject *parent)
    : QObject(parent)
{
//    connect(this, &SubscriptionManager::triggerTask, this, &SubscriptionManager::askModelForTask,
//            Qt::QueuedConnection);
}

void SubscriptionManager::registerCreator(Model* creator, QString dataId,
                                          std::vector<QString> dependencies) {

    //assert(dataPool.find(dataId) == dataPool.end());

    dataPool[dataId].data_handle = handle(new boost::dynamic_any());
    dataPool[dataId].meta_handle = handle(new boost::dynamic_any());

    dataPool[dataId].creator = creator;

    for(QString& s : dependencies) {
        dataPool[s].dependants.push_back(dataId);
    }
}

void SubscriptionManager::subscribe(QString dataId, SubscriptionType sub,
                                    int version, Subscription* subObj)
{
    dataPool[dataId].currentSubs[subObj->getId()] = subObj;

    if(version != -1) {
        dataPool[dataId].subscribedVersions.push(version);
    }

    if (sub == SubscriptionType::READ) subscribeRead(dataId, subObj);
    else subscribeWrite(dataId);

    updateState(dataId);
}

void SubscriptionManager::subscribeRead(QString dataId, Subscription* subObj)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    bool instantRequest = subObj->getSubscriberType() == SubscriberType::READER;
    dataPool[dataId].willReads++;
    if (dataPool[dataId].validity == ValidityState::INVALID
            && !dataPool[dataId].willWrite) {
        askModelForTask(dataId);
        //emit triggerTask(dataId);
    } else if (dataPool[dataId].access != AccessState::WRITE && instantRequest
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

    propagateChange(dataId);
    invalidDependants(dataId);
}

void SubscriptionManager::unsubscribe(QString dataId, SubscriptionType sub,
                                      int version, Subscription *subObj, bool consumed)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    if(!dataPool[dataId].subscribedVersions.empty()
            && version == dataPool[dataId].subscribedVersions.top()) {
        dataPool[dataId].subscribedVersions.pop();
    }

    dataPool[dataId].currentSubs.erase(subObj->getId());
    if (sub == SubscriptionType::READ) dataPool[dataId].willReads--;
    else {
        dataPool[dataId].willWrite = false;
        updateState(dataId);
        if (consumed) sendUpdate(dataId);
        //propagateChange(dataId);
    }

    if(!dataPool[dataId].willWrite && dataPool[dataId].willReads == 0) {
        dataPool[dataId].data_handle = handle(new boost::dynamic_any());
        dataPool[dataId].meta_handle = handle(new boost::dynamic_any());

        dataPool[dataId].initialized = false;
        dataPool[dataId].upToDate = false;
        dataPool[dataId].validity = ValidityState::INVALID;

        qDebug() << "data" << dataId << "was released due to no subscribers!";
    }

}

handle_tuple SubscriptionManager::doSubscription(QString id, SubscriptionType sub) {
    if (sub == SubscriptionType::READ) return doReadSubscription(id);
    else return doWriteSubscription(id);
}

handle_tuple SubscriptionManager::doReadSubscription(QString id)
{
    auto data = dataPool[id].read();

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doReads++;
    updateState(id);

    return data;
}

handle_tuple SubscriptionManager::doWriteSubscription(QString id)
{
    auto data = dataPool[id].write();

    std::unique_lock<std::recursive_mutex> lock(mu);
    //dataPool[id].upToDate = false;
    dataPool[id].doWrite = true;

    updateState(id);
    return data;
}

void SubscriptionManager::endDoSubscription(QString id, SubscriptionType sub)
{
    if (sub == SubscriptionType::READ) endDoReadSubscription(id);
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
    //invalidDependants(id);

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doWrite = false;
    dataPool[id].upToDate = true;
    dataPool[id].initialized = true;
    dataPool[id].internalVersion++;
    dataPool[id].endWrite();

    updateState(id);
//    sendUpdate(id);
//    propagateChange(id);
}

void SubscriptionManager::propagateChange(QString id) {

    for(QString data: dataPool[id].dependants) {
        if (hasWillReads(data) && !dataPool[data].willWrite) {
        //if (dataPool[data].willReads > 0 && !dataPool[data].willWrite) {
            askModelForTask(data, id);
            //dataToAsk.push_back(data);
            //emit triggerTask(data);
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

    if (dataPool[id].upToDate && dataPool[id].initialized) dataPool[id].validity = ValidityState::VALID;
    else dataPool[id].validity = ValidityState::INVALID;

    if (!dataPool[id].doWrite && dataPool[id].doReads == 0)
    {
        dataPool[id].access = AccessState::NONE;
    }
    else if (dataPool[id].doWrite && dataPool[id].doReads == 0)
    {
        dataPool[id].access = AccessState::WRITE;
    } else if (!dataPool[id].doWrite && dataPool[id].doReads > 0)
    {
        dataPool[id].access = AccessState::READ;
    } else {
        assert(false);
    }
}

bool SubscriptionManager::isDataInitialized(QString dataId)
{
    return dataPool[dataId].initialized && dataPool[dataId].upToDate;
}

void SubscriptionManager::sendUpdate(QString id)
{
    auto subs = dataPool[id].currentSubs;
    for(auto sub : subs) {
        sub.second->update();
    }
}

bool SubscriptionManager::hasWillReads(QString parentId) {

    if (dataPool[parentId].willReads > 0)
        return true;

    auto& deps = dataPool[parentId].dependants;

    return std::any_of(deps.begin(), deps.end(), [this](QString& data) {
        return dataPool[data].willReads > 0 || hasWillReads(data);
    });
}

void SubscriptionManager::askModelForTask(QString requestedId, QString beingComputedId)
{
    qDebug() << "asking for" << requestedId;
    dataPool[requestedId].creator->delegateTask(requestedId, beingComputedId);
}

bool SubscriptionManager::processDependencies(std::vector<Dependency> &dependencies)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    for(Dependency& dep : dependencies) {

        QString dataId = dep.dataId;

        if(!dataPool[dataId].subscribedVersions.empty()) {
            int smallestSubscribedVersion = dataPool[dataId].subscribedVersions.top();
            int currentVersion = dataPool[dataId].externalVersion;
            if ( ( dep.version == -1 && smallestSubscribedVersion == currentVersion )
                    || dep.version > smallestSubscribedVersion) return false;
        }

//        if (dep.subscription == SubscriptionType::READ
//                && (dataPool[dataId].access == AccessState::WRITE
//                    || dataPool[dataId].validity != ValidityState::VALID))
//            return false;

        if (dep.subscription == SubscriptionType::READ)
        {
            if (dataPool[dataId].access == AccessState::WRITE) return false;
            if (dataPool[dataId].validity != ValidityState::VALID
                    && dep.version != dataPool[dataId].externalVersion)
                return false;
        }

        if (dep.subscription == SubscriptionType::WRITE
                && dataPool[dataId].access != AccessState::NONE)
            return false;
    }
    return true;
}
