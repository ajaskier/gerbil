#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QDebug>
#include <memory>
#include "dependency.h"

class SubscriptionManager;
class TaskScheduler;
enum class SubscriptionType;
class Subscription;


class Task
{

public:
    explicit Task(QString id) : id(id) {}
    virtual ~Task() {
        qDebug() << "deleting task" << id;
    }
    virtual void start() {
        run();
    }

    std::vector<Dependency>& getDependencies() {
        return dependencies;
    }
    QString getId() { return id; }
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) = 0;


protected:
    virtual void run() = 0;

    QString id;
    std::vector<Dependency> dependencies;

};

#endif // TASK_H
