#ifndef TASK_IMAGE_IMG_H
#define TASK_IMAGE_IMG_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>

class TaskImageIMG : public Task
{
    Q_OBJECT
public:
    explicit TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                          bool includecache = true, QObject* parent = nullptr);
    virtual ~TaskImageIMG();
    virtual void run() override;

private:

    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;
    void createSubscriptions(Task* task);

    std::unique_ptr<Subscription> sourceSub;
    std::unique_ptr<Subscription> targetSub;

    //scope
    cv::Rect roi;

    //rescale
    size_t bands;
    size_t roiBands;
    //size_t newsize;
    bool includecache;
};


#endif // TASK_IMAGE_IMG_H
