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
    explicit TaskScopeImage(cv::Rect roi, QObject* parent = nullptr);
    virtual ~TaskScopeImage();

    virtual void run() override;

private:

    virtual void setSubscription(QString id,
                                 std::unique_ptr<Subscription> sub) override;

    std::unique_ptr<Subscription> source;
    std::unique_ptr<Subscription> target;
    cv::Rect roi;
};


#endif // TASK_SCOPE_IMAGE_H
