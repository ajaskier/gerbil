#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QDebug>
#include "dependency.h"

class SubscriptionManager;
class TaskScheduler;
enum class SubscriptionType;
class Subscription;


class Task : public QObject
{
    Q_OBJECT

public:
    explicit Task(QString id, QObject *parent = 0)
        : QObject(parent), id(id) {}
    virtual ~Task() {
        qDebug() << "deleting task" << id;
    }
    virtual void start() {
        run();
        emit finished(this);
    }
    std::vector<Dependency>& getDependencies() {
        return dependencies;
    }
    QString getId() { return id; }
    virtual void setSubscription(QString id, Subscription* sub) = 0;
    virtual void endSubscriptions() = 0;

signals:
    void finished(Task* task);

protected:
    virtual void run() = 0;

    QString id;
    std::vector<Dependency> dependencies;

};

#endif // TASK_H
