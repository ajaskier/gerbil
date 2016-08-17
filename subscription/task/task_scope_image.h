#ifndef TASK_SCOPE_IMAGE_H
#define TASK_SCOPE_IMAGE_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include "multi_img.h"

class TaskScopeImage : public Task
{

public:
    explicit TaskScopeImage(cv::Rect roi);
    virtual ~TaskScopeImage();

    virtual bool run() override;
    virtual void setSubscription(QString id,
                                 std::shared_ptr<Subscription> sub) override;

private:

    std::shared_ptr<Subscription> source;
    std::shared_ptr<Subscription> target;
    cv::Rect roi;
};


#endif // TASK_SCOPE_IMAGE_H
