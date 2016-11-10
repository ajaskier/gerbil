#ifndef TASK_ROI_H
#define TASK_ROI_H

#include <QObject>
#include <task/task.h>

#include <opencv2/core/core.hpp>

class TaskRoi : public Task
{

public:
    explicit TaskRoi(cv::Rect oldRoi, cv::Rect newRoi);

    virtual ~TaskRoi();
    virtual bool run() override;

private:

    cv::Rect oldRoi;
    cv::Rect newRoi;

};


#endif // TASK_ROI_H
