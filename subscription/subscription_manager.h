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
#include "state.h"

class Model;
class Dependency;

//using any_sptr = std::shared_ptr<boost::dynamic_any>;

class SubscriptionManager : public QObject
{
    Q_OBJECT
public:

    explicit SubscriptionManager(QObject* parent = nullptr);
    void registerCreator(Model* creator, QString dataId, std::vector<QString> dependencies);
    void subscribe(QString dataId, SubscriptionType sub, Subscription* subObj);
    void unsubscribe(QString dataId, SubscriptionType sub, Subscription* subObj);

signals:
    void triggerTask(QString id);

private:

    void askModelForTask(QString id);
    void updateState(QString id);
    bool processDependencies(std::vector<Dependency>& dependencies);
    void sendUpdate(QString id);
    bool hasWillReads(QString parentId);
    void invalidDependants(QString id);

    void propagateChange(QString id);

    void subscribeRead(QString dataId, Subscription *subObj);
    void subscribeWrite(QString dataId);

    handle_pair doSubscription(QString id, SubscriptionType sub);
    handle_pair doReadSubscription(QString id);
    handle_pair doWriteSubscription(QString id);

    void endDoSubscription(QString id, SubscriptionType sub);
    void endDoReadSubscription(QString id);
    void endDoWriteSubscription(QString id);

    std::map<QString, DataEntry> dataPool;
    std::recursive_mutex mu;

    friend class TaskScheduler;
    friend class Subscription;
};


#endif // SUBSCRIPTION_MANAGER_H
