#ifndef TASK_IMAGE_IMG_H
#define TASK_IMAGE_IMG_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>

class TaskImageIMG : public Task
{

public:
    explicit TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                          bool includecache = true);
    virtual ~TaskImageIMG();
    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    std::shared_ptr<Subscription> sourceSub;
    std::shared_ptr<Subscription> targetSub;

    //scope
    cv::Rect roi;

    //rescale
    size_t bands;
    size_t roiBands;
    //size_t newsize;
    bool includecache;
};


#endif // TASK_IMAGE_IMG_H
