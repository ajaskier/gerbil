#ifndef TASK_IMAGE_REPRESENTATION_IMG_H
#define TASK_IMAGE_REPRESENTATION_IMG_H

#include <QObject>
#include <task/task.h>

class TaskImageRepresentationImg : public Task
{
    Q_OBJECT
public:
    explicit TaskImageRepresentationImg(QObject* parent = nullptr);
    virtual ~TaskImageRepresentationImg();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    Subscription* sub;
};


#endif // TASK_IMAGE_REPRESENTATION_IMG_H
