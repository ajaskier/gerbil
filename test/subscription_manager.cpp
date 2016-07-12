#include "subscription_manager.h"
#include <QDebug>

#include <model/model.h>

void SubscriptionManager::registerCreator(Model *creator,
                                          DataId dataId)
{

    if(dataId == DataId::DATA_A) {

        dataPool[dataId].handle = std::shared_ptr<SpecificType<Data>>(
            new SpecificType<Data>(dataId, Data()));

    } else if(dataId == DataId::DATA_B) {

        dataPool[dataId].handle = std::shared_ptr<SpecificType<Data>>(
            new SpecificType<Data>(dataId, Data()));
    }
    dataPool[dataId].creator = creator;
    qDebug() << "registered" << toString(dataId);
}

void SubscriptionManager::registerWill(QObject *subscriber, DataId dataId,
                                       Subscription sub) {
    dataPool[dataId].subscribers[subscriber] = sub;
}

std::shared_ptr<MultiType> SubscriptionManager::doSubscription(DataId id,
                                                               Subscription sub) {

    //if it is DO_READ increment doReaders - sanity check if state is VALID or READ and doWrite is false
    //if it is DO_WRITE set doWrite to true - sanity check if

    if(sub == Subscription::DO_READ) {
        dataPool[id].doReads++;
    } else if(sub == Subscription::DO_WRITE) {
        dataPool[id].doWrite = true;
    }
    updateState(id);

    return dataPool[id].handle;
}

void SubscriptionManager::endDoSubscription(DataId id, Subscription sub)
{
    dataPool[id].state = State::VALID;

    if(sub == Subscription::DO_READ) {
        dataPool[id].doReads--;
    } else if(sub == Subscription::DO_WRITE) {
        dataPool[id].doWrite = false;

        //set dependants to invalid!
    }

    updateState(id);
    //if it was DO_WRITE then change state to valid
    //and set dependants to invalid

    //if it was DO_READ then decrement doReaders
}

void SubscriptionManager::updateState(DataId id)
{
    if(dataPool[id].doWrite == false
            && dataPool[id].doReads == 0)
    {
        dataPool[id].state = State::VALID;
    } else if(dataPool[id].doWrite == true
               && dataPool[id].doReads == 0)
    {
        dataPool[id].state = State::WRITE;
    } else if(dataPool[id].doWrite == false
              && dataPool[id].doReads > 0)
    {
        dataPool[id].state = State::READ;
    } else {
        throw "state corrupted";
    }


}

bool SubscriptionManager::isInState(DataId data, State state)
{
    return dataPool[data].state == state;
}

void SubscriptionManager::askModelForTask(DataId id)
{
    dataPool[id].creator->delegateTask();
}
