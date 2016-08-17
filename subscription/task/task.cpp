#include "task.h"

#include <QDebug>
#include <cassert>
#include "subscription.h"

Task::Task(QString target, std::map<QString, QString> sources)
    : id(target), sources(sources)
{

    for (auto source : sources) {
        dependencies.push_back(Dependency(source.first, SubscriptionType::READ));
        subscriptions[source.second] = std::shared_ptr<Subscription>();
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
    QString innerId = sources[id];
    subscriptions[innerId] = sub;
}

bool Task::start() {
    return run();
}

std::shared_ptr<Subscription> Task::sub(QString id) {
    auto it = subscriptions.find(id);

    assert(it != subscriptions.end());
    assert(it->second);

    return it->second;
}
