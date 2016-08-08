#ifndef TASK_SCOPE_IMAGE_H
#define TASK_SCOPE_IMAGE_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>
#include "multi_img.h"

class TaskScopeImage : public Task
{
    Q_OBJECT
public:
    explicit TaskScopeImage(Subscription* imgSub, cv::Rect roi, std::shared_ptr<multi_img> target,
                            QObject* parent = nullptr);
    virtual ~TaskScopeImage();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    Subscription* imgSub;
    cv::Rect roi;
    std::shared_ptr<multi_img> target;
};


#endif // TASK_SCOPE_IMAGE_H
