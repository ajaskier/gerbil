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

}

void SubscriptionManager::subscribeRead(QString dataId, Subscription* subObj)
{
    std::unique_lock<std::recursive_mutex> lock(mu);

    bool updateRequest = subObj->getAccessType() == AccessType::DEFERRED;
    dataPool[dataId].willReads++;

    if (!isValid(dataId) && !hasWillWrite(dataId)) {
        askModelForTask(dataId);
    } else if (!isWriteState(dataId) && updateRequest
              && dataPool[dataId].upToDate) {
        sendUpdate(dataId, subObj->getId());
    }
}

void SubscriptionManager::subscribeWrite(QString dataId)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[dataId].willWrite = true;
    dataPool[dataId].upToDate = false;

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
    if (sub == SubscriptionType::READ) {
        dataPool[dataId].willReads--;
    } else {
        dataPool[dataId].willWrite = false;

        qDebug() << "WRITE OF DATA" << dataId << "ENDED. CURRENT VERSION: "
                 << dataPool[dataId].majorVersion;

        if (consumed && !dataPool[dataId].data_handle->empty()) {
            sendUpdate(dataId);
        }
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

    return data;
}

handle_pair SubscriptionManager::doWriteSubscription(QString id)
{
    auto data = dataPool[id].write();

    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].upToDate = false;
    dataPool[id].doWrite = true;

    return data;
}

int SubscriptionManager::getMinorVersion(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[id].getMinorVersion();
}

int SubscriptionManager::getMajorVersion(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[id].getMajorVersion();
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
}

void SubscriptionManager::endDoWriteSubscription(QString id)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    dataPool[id].doWrite = false;
    dataPool[id].upToDate = true;
    dataPool[id].initialized = true;
    dataPool[id].minorVersion++;
    dataPool[id].endWrite();

}

void SubscriptionManager::propagateChange(QString id) {

    for (QString data: dataPool[id].dependants) {
        if (hasWillReadsRecursive(data) && !dataPool[data].willWrite) {
            askModelForTask(data, id);
        } else if(!hasWillReadsRecursive(data) && !dataPool[data].willWrite) {
            removeData(data);
        }
    }
}

void SubscriptionManager::invalidDependants(QString id)
{
    for(QString& data: dataPool[id].dependants) {
        //qDebug() << "invalidating" << data;
        dataPool[data].upToDate = false;
        invalidDependants(data);
    }
}

bool SubscriptionManager::isDataInitialized(QString dataId)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[dataId].initialized;// && dataPool[dataId].upToDate;
}

bool SubscriptionManager::isUpToDate(QString dataId)
{
    std::unique_lock<std::recursive_mutex> lock(mu);
    return dataPool[dataId].upToDate;
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

bool SubscriptionManager::hasWillReadsRecursive(QString id) {

    if (dataPool[id].willReads > 0)
        return true;

    auto& deps = dataPool[id].dependants;

    return std::any_of(deps.begin(), deps.end(), [this](QString& data) {
        return dataPool[data].willReads > 0 || hasWillReadsRecursive(data);
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

            if (( dep.version == -1 && smallestSubscribedVersion == currentVersion )
                    || dep.version > smallestSubscribedVersion)
                return false;
            else if (dep.version == -1 || dep.version == currentVersion)
                continue;
            else
                return false;
        }

        if (dep.subscription == SubscriptionType::READ)
        {
            if (isWriteState(dataId)) return false;
            if (dep.accessType == AccessType::FORCED) {
                //EXPERIMENTAL!!! AND HACK!!!
                qDebug() << "Granting immediate access for" << dep.dataId;
                continue;
            }
            if (!isValid(dataId) && dep.version != dataPool[dataId].majorVersion)
                return false;
        }

        if (dep.subscription == SubscriptionType::WRITE
                && !isNoneState(dataId))
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

   // qDebug() << "data" << dataId << "was released due to no subscribers!";
}

bool SubscriptionManager::isReadState(QString id)
{
    return !dataPool[id].doWrite && dataPool[id].doReads > 0;
}

bool SubscriptionManager::isWriteState(QString id)
{
    return dataPool[id].doWrite && dataPool[id].doReads == 0;
}

bool SubscriptionManager::isNoneState(QString id)
{
    return !dataPool[id].doWrite && dataPool[id].doReads == 0;
}

bool SubscriptionManager::isValid(QString id)
{
    return dataPool[id].upToDate && dataPool[id].initialized;
}

bool SubscriptionManager::hasWillReads(QString id)
{
    return dataPool[id].willReads > 0;
}

bool SubscriptionManager::hasWillWrite(QString id)
{
    return dataPool[id].willWrite;
}
