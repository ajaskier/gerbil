#ifndef MODEL_D_H
#define MODEL_D_H

#include <QObject>
#include <subscription_manager.h>
#include <model/model.h>

class ModelD : public Model
{
    Q_OBJECT
public:
    explicit ModelD(int d, SubscriptionManager& sm, TaskScheduler *scheduler,
                    QObject *parent = 0);

public slots:
    virtual void delegateTask(QString id, QString parentId = "") override;
    void setD(int d) { this->d = d; }

protected:
    int d;

};

#endif // MODEL_D_H
