#include "task.h"

#include <QDebug>
#include <cassert>
#include "subscription.h"

Task::Task(QString target, std::map<QString, QString> sources)
    : id(target), sources(sources)
{

    std::vector<QString> badKeys;
    for (auto& source : this->sources) {

        QString dataString = source.first;
        QStringList list = dataString.split("-");

        if(list.size() > 1) {
            badKeys.push_back(source.first);
            int version = list[1].toInt();
            dependencies.push_back(Dependency(list[0], SubscriptionType::READ, version));

        } else {
            dependencies.push_back(Dependency(list[0], SubscriptionType::READ));

        }

        //dependencies.push_back(Dependency(source.first, SubscriptionType::READ));
        subscriptions[source.second] = std::shared_ptr<Subscription>();
    }

    for(QString key : badKeys) {

        QString newKey = key.split("-").at(0);

        auto it = this->sources.find(key);
        if (it != this->sources.end() ) {
            std::swap(this->sources[newKey], it->second);
            this->sources.erase(it);

        }
    }

    dependencies.push_back(Dependency(target, SubscriptionType::WRITE));
    subscriptions["dest"] = std::shared_ptr<Subscription>();
    this->sources[target] = "dest";
}

Task::Task(QString id, QString target, std::map<QString, QString> sources)
    : id(id), sources(sources)
{

    std::vector<QString> badKeys;
    for (auto source : sources) {

        QString dataString = source.first;
        QStringList list = dataString.split("-");

        if(list.size() > 1) {
            badKeys.push_back(source.first);
            int version = list[1].toInt();
            dependencies.push_back(Dependency(list[0], SubscriptionType::READ, version));
        } else {
            dependencies.push_back(Dependency(list[0], SubscriptionType::READ));
        }

        //dependencies.push_back(Dependency(source.first, SubscriptionType::READ));
        subscriptions[source.second] = std::shared_ptr<Subscription>();
    }

    for(QString key : badKeys) {

        QString newKey = key.split("-").at(0);

        auto it = this->sources.find(key);
        if (it != this->sources.end() ) {
            std::swap(this->sources[newKey], it->second);
            this->sources.erase(it);

        }
    }

    dependencies.push_back(Dependency(target, SubscriptionType::WRITE));
    subscriptions["dest"] = std::shared_ptr<Subscription>();
    this->sources[target] = "dest";
}

Task::~Task()
{
    qDebug() << "deleting task" << id;
}


void Task::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    auto list = id.split("-");
    if(list.size() > 1) {
        id = list[0];
    }

    QString innerId = sources[id];
    subscriptions[innerId] = sub;
}

bool Task::start() {
    bool success = run();
    emit finished(this->id, success);
    return success;
}

void Task::invalidateSubscriptions()
{
    for (auto& s : subscriptions) {
        s.second->forceUnsubscribe();
    }
}

bool Task::subExists(QString id)
{
    return subscriptions.find(id) != subscriptions.end();
}

std::shared_ptr<Subscription> Task::sub(QString id) {
    auto it = subscriptions.find(id);

    assert(it != subscriptions.end());
    assert(it->second);

    return it->second;
}
