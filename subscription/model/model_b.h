#ifndef MODEL_B_H
#define MODEL_B_H

#include <QObject>
#include <subscription_manager.h>
#include <model/model.h>

class ModelB : public Model
{
    Q_OBJECT
public:
	explicit ModelB(int b, int c, TaskScheduler *scheduler, QObject *parent = 0);

public slots:
    virtual void delegateTask(QString id, QString parentId = "") override;
    void setB(int b) { this->b = b; }
    void setC(int c) { this->c = c; }

protected:
    int b;
    int c;

};

#endif // MODEL_B_H
