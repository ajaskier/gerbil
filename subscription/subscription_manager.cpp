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

    bool instantRequest = subObj->getAccessType() == AccessType::DEFERRED;
    dataPool[dataId].willReads++;
    if (dataPool[dataId].validity == ValidityState::INVALID
            && !dataPool[dataId].willWrite) {
        askModelForTask(dataId);
        //emit triggerTask(dataId, "");
    } else if (dataPool[dataId].access != AccessState::WRITE && instantRequest
              && dataPool[dataId].upToDate) {
        sendUpdate(dataId, subObj->getId());
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

        qDebug() << "WRITE OF DATA" << dataId << "ENDED. CURRENT VERSION: "
                 << dataPool[dataId].majorVersion;

        updateState(dataId);
        if (consumed && !dataPool[dataId].data_handle->empty()) {
            sendUpdate(dataId);
        } else {
            qDebug() << "signal won't be propagated because subscription wasn't consumed or data is empty";
        }
        //propagateChange(dataId);
    }

    if(!dataPool[dataId].willWrite && dataPool[dataId].willReads == 0) {

        if (dataPool[dataId].upToDate) {
            qDebug() << "data" << dataId << "would be erased but it's valid";
        } else {
            removeData(dataId);
        }
    }
}

handle_pair SubscriptionManager::doSubscription(QString id, SubscriptionType sub) {
    if (sub == SubscriptionType::READ) return doReadSubscription(id);
    else return doWriteSubscription(id);
}

handle_pair SubscriptionManager::doReadSubscription(QString id)
{
    auto data = dataPool[id].read();

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doReads++;
    updateState(id);

    return data;
}

handle_pair SubscriptionManager::doWriteSubscription(QString id)
{
    auto data = dataPool[id].write();

    std::unique_lock<std::recursive_mutex> lock(mu);
    //dataPool[id].upToDate = false;
    dataPool[id].doWrite = true;

    updateState(id);
    return data;
}

int SubscriptionManager::getMinorVersion(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[id].getMinorVersion();
}

int SubscriptionManager::getMajorVersion(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);    return dataPool[id].getMajorVersion();
}

void SubscriptionManager::setMajorVersion(QString id, int version)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    qDebug() << "setting major version of " << id << "to " << version;
    if (dataPool[id].getMajorVersion() < version) dataPool[id].minorVersion = 0;
    dataPool[id].getMajorVersion() = version;
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
    dataPool[id].minorVersion++;
    dataPool[id].endWrite();

    updateState(id);
//    sendUpdate(id);
//    propagateChange(id);
}

void SubscriptionManager::propagateChange(QString id) {

    for (QString data: dataPool[id].dependants) {
        if (hasWillReads(data) && !dataPool[data].willWrite) {
            askModelForTask(data, id);
        } else if(dataPool[data].initialized) {
            removeData(data);
        }
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
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[dataId].initialized;// && dataPool[dataId].upToDate;
}

void SubscriptionManager::sendUpdate(QString id)
{
    qDebug() << "update signal of" << id << "on unsubscribe";
    auto subs = dataPool[id].currentSubs;
    for(auto sub : subs) {
        sub.second->update();
    }
}

void SubscriptionManager::sendUpdate(QString id, int subscriberId)
{
    qDebug() << "direct update signal of" << id;
    auto sub = dataPool[id].currentSubs[subscriberId];
    sub->update();
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
    qDebug() << "asking for" << requestedId << "from" << beingComputedId;
    dataPool[requestedId].creator->delegateTask(requestedId, beingComputedId);
}

bool SubscriptionManager::processDependencies(std::vector<Dependency> &dependencies)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    for(Dependency& dep : dependencies) {

        QString dataId = dep.dataId;

        if(!dataPool[dataId].subscribedVersions.empty()) {

            int smallestSubscribedVersion = dataPool[dataId].subscribedVersions.top();
            int currentVersion = dataPool[dataId].majorVersion;

//            if (currentVersion < smallestSubscribedVersion
//                    && dep.subscription == SubscriptionType::READ) {
//                askModelForTask(dataId);
//                return false;
//            }

            if (( dep.version == -1 && smallestSubscribedVersion == currentVersion )
                    || dep.version > smallestSubscribedVersion/*
                    || dep.version != currentVersion*/) return false;
            else if (dep.version == -1 || dep.version == currentVersion)
                return true;
            else
                return false;
        }

//        if (dep.subscription == SubscriptionType::READ
//                && (dataPool[dataId].access == AccessState::WRITE
//                    || dataPool[dataId].validity != ValidityState::VALID))
//            return false;

        if (dep.subscription == SubscriptionType::READ)
        {
            if (dataPool[dataId].access == AccessState::WRITE) return false;
            if (dataPool[dataId].validity != ValidityState::VALID
                    && dep.version != dataPool[dataId].majorVersion)
                return false;
        }

        if (dep.subscription == SubscriptionType::WRITE
                && dataPool[dataId].access != AccessState::NONE)
            return false;
    }
    return true;
}

void SubscriptionManager::removeData(QString dataId)
{
    dataPool[dataId].data_handle = handle(new boost::dynamic_any());
    dataPool[dataId].meta_handle = handle(new boost::dynamic_any());

    dataPool[dataId].initialized = false;
    dataPool[dataId].upToDate = false;
    dataPool[dataId].validity = ValidityState::INVALID;

    qDebug() << "data" << dataId << "was released due to no subscribers!";
}
