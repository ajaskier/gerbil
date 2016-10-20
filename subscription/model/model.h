#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <utility>
#include "subscription_manager.h"

class TaskScheduler;
enum class SubscriptionType;

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(SubscriptionManager& sm, TaskScheduler* scheduler,
                   QObject *parent = 0)
        : QObject(parent), sm(sm), scheduler(scheduler) {}
    virtual ~Model() {}

public slots:
    virtual void delegateTask(QString requestedId, QString parentId = "") = 0;

protected:

    void registerData(QString dataId, std::vector<QString> dependencies) {
        sm.registerCreator(this, dataId, dependencies);
    }

    SubscriptionManager& sm;
    TaskScheduler* scheduler;

};

#endif // MODEL_H
