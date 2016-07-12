#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <subscription_manager.h>

#include <model/model_a.h>
#include <model/model_b.h>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    virtual ~Controller() {}
    void init();
private:

    //SubscriptionManager* sm;

    ModelA* modelA;
    ModelB* modelB;
};

#endif // CONTROLLER_H
