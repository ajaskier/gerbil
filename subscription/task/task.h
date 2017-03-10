#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <memory>
#include <QString>
#include "dependency.h"
#include <map>
#include "source_declaration.h"

class Subscription;

class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(QString target, std::map<QString, SourceDeclaration> sources);
    explicit Task(QString id, QString target, std::map<QString, SourceDeclaration> sources);

    virtual ~Task();
    virtual bool start() final;
	virtual void importSubscription(std::shared_ptr<Subscription> sub) final;
	virtual void invalidateSubscriptions() final;

    std::vector<Dependency>& getDependencies() { return dependencies; }
    QString getId() { return id; }

signals:
    void taskFinished(QString id, bool success);

protected:
    virtual bool run() = 0;
    virtual std::shared_ptr<Subscription> sub(QString id) final;
    virtual bool subExists(QString id) final;
    virtual bool isCancelled() { return false; }

private:

    void setDependencies(QString target);

    QString id;
    std::vector<Dependency> dependencies;
    std::map<QString, SourceDeclaration> sources;
    std::map<QString, std::shared_ptr<Subscription>> subscriptions;

};

#endif // TASK_H
