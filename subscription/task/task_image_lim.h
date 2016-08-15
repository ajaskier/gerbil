#ifndef TASK_IMAGE_LIM_H
#define TASK_IMAGE_LIM_H

#include <QObject>
#include <task/task.h>

class TaskImageLim : public Task
{
    Q_OBJECT
public:
    explicit TaskImageLim(const QString &filename, bool limitedMode,
                          QObject* parent = nullptr);
    virtual ~TaskImageLim();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    const QString& filename;
    bool limitedMode;
    std::shared_ptr<Subscription> sub;
};


#endif // TASK_IMAGE_LIM_H
