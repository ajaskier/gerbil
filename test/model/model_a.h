#ifndef MODEL_A_H
#define MODEL_A_H

#include <QObject>
#include <memory>
#include <model/model.h>

class TaskA : public Task
{
    Q_OBJECT
public:
    explicit TaskA(int a, QObject* parent = 0);

    virtual void run() override;

private:
    int a;
};


class ModelA : public Model
{
    Q_OBJECT
public:
    explicit ModelA(int a, QObject *parent = 0);

public slots:
    virtual void delegateTask() override;

private:

    int a;
};

#endif // MODEL_A_H
