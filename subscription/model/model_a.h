#ifndef MODEL_A_H
#define MODEL_A_H

#include <QObject>
#include <memory>
#include <model/model.h>

class ModelA : public Model
{
    Q_OBJECT
public:
    explicit ModelA(int a, SubscriptionManager& sm, TaskScheduler* scheduler,
                    QObject *parent = 0);

public slots:
    virtual void delegateTask(QString id, QString parentId = "") override;
    void setA(int a) { this->a = a; }

protected:
    int a;
};

#endif // MODEL_A_H
