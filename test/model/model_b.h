#ifndef MODEL_B_H
#define MODEL_B_H

#include <QObject>
#include <subscription_manager.h>
#include <model/model.h>

class TaskB : public Task
{
    Q_OBJECT
public:
    explicit TaskB(int b, QObject* parent = 0);

    virtual void run() override;

private:
    int b;
};

class ModelB : public Model
{
    Q_OBJECT
public:
    explicit ModelB(int b, QObject *parent = 0);

    virtual void delegateTask() override;

private:
    int b;

};

#endif // MODEL_B_H
