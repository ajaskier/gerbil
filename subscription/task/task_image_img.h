#ifndef TASK_IMAGE_IMG_H
#define TASK_IMAGE_IMG_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>
#include <tbb/tbb.h>

class TaskImageIMG : public Task
{
    Q_OBJECT
public:
    explicit TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                          bool includecache = true, QObject* parent = nullptr);
    virtual ~TaskImageIMG();

    virtual void run() override;
    virtual void endSubscriptions() override;

private:

    virtual void setSubscription(QString id, Subscription *sub) override;

    Subscription* imgSub;
    Subscription* imgIMGSub;

    //scope
    cv::Rect roi;

    //rescale
    size_t bands;
    size_t roiBands;
    size_t newsize;
    bool includecache;
};


#endif // TASK_IMAGE_IMG_H
