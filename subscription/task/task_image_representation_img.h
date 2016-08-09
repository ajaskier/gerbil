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

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    std::unique_ptr<Subscription> sub;
};


#endif // TASK_IMAGE_REPRESENTATION_IMG_H
