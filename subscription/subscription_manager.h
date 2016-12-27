#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <QObject>
#include <memory>
#include <functional>
#include <utility>
#include <cassert>
#include <mutex>

#include "data_entry.h"
#include "subscription_type.h"

class Model;
class Dependency;

//using any_sptr = std::shared_ptr<boost::dynamic_any>;

class SubscriptionManager : public QObject
{
    Q_OBJECT
public:

    explicit SubscriptionManager(QObject* parent = nullptr);
    void registerCreator(Model* creator, QString dataId, std::vector<QString> dependencies);
    void subscribe(QString dataId, SubscriptionType sub, int version, Subscription* subObj);
    void unsubscribe(QString dataId, SubscriptionType sub, int version, Subscription* subObj,
                     bool consumed = true);
    bool isDataInitialized(QString dataId);

private:

    inline bool isReadState(QString id);
    inline bool isWriteState(QString id);
    inline bool isNoneState(QString id);
    inline bool isValid(QString id);

    inline bool hasWillWrite(QString id);
    inline bool hasWillReads(QString id);
    bool hasWillReadsRecursive(QString id);

    void subscribeRead(QString dataId, Subscription *subObj);
    void subscribeWrite(QString dataId);

    handle_pair doSubscription(QString id, SubscriptionType sub);
    handle_pair doReadSubscription(QString id);
    handle_pair doWriteSubscription(QString id);

    void endDoSubscription(QString id, SubscriptionType sub);
    void endDoReadSubscription(QString id);
    void endDoWriteSubscription(QString id);

    int getMinorVersion(QString id);
    int getMajorVersion(QString id);
    void setMajorVersion(QString id, int version);

    bool processDependencies(std::vector<Dependency>& dependencies);

    void askModelForTask(QString requestedId, QString beingComputedId = "");
    void updateState(QString id);
    void sendUpdate(QString id);
    void sendUpdate(QString id, int subscriberId);
    void invalidDependants(QString id);

    void removeData(QString dataId);
    void propagateChange(QString id);

    std::map<QString, DataEntry> dataPool;
    std::recursive_mutex mu;

    friend class TaskScheduler;
    friend class Subscription;
    friend class DataConditionInformer;
};


#endif // SUBSCRIPTION_MANAGER_H
