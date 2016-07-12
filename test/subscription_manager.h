#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <QObject>
#include <memory>
#include <functional>
#include <multitype.h>
#include "subscription_utils.h"

class Model;
enum class DataId;

class SubscriptionManager : public QObject
{
    Q_OBJECT
public:

    void registerCreator(Model* creator, DataId dataId);
    void registerWill(QObject *subscriber, DataId dataId, Subscription sub);

    std::shared_ptr<MultiType> doSubscription(DataId id, Subscription sub);
    void endDoSubscription(DataId id, Subscription sub);

    static SubscriptionManager& instance() {
        static SubscriptionManager sm;
        return sm;
    }
    SubscriptionManager(SubscriptionManager const&) = delete;
    void operator=(SubscriptionManager const&) = delete;

private:
    explicit SubscriptionManager() : QObject() {}

    bool isInState(DataId data, State state);
    void askModelForTask(DataId id);
    void updateState(DataId id);

    std::map<DataId, DataEntry> dataPool;
    friend class TaskScheduler;
};

#endif // SUBSCRIPTION_MANAGER_H
