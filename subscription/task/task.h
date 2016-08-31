#ifndef TASK_H
#define TASK_H

#include <memory>
#include <QString>
#include "dependency.h"
#include <map>

class Subscription;

class Task
{

public:
    explicit Task(QString target, std::map<QString, QString> sources);

    virtual ~Task();
    virtual bool start() final;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) final;

    std::vector<Dependency>& getDependencies() { return dependencies; }
    QString getId() { return id; }

protected:
    virtual bool run() = 0;
    virtual std::shared_ptr<Subscription> sub(QString id) final;
    virtual bool isCancelled() { return false; }

private:

    QString id;
    std::vector<Dependency> dependencies;
    std::map<QString, QString> sources;
    std::map<QString, std::shared_ptr<Subscription>> subscriptions;

};

#endif // TASK_H
