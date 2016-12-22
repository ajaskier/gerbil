#include "task_roi.h"
#include "subscription.h"
#include "lock.h"
#include "data.h"
#include "QDebug"

#include <rectangles.h>

#define REUSE_THRESHOLD 0.1

TaskRoi::TaskRoi(cv::Rect oldRoi, cv::Rect newRoi)
    : Task("ROI", {}), oldRoi(oldRoi), newRoi(newRoi)
{
}



TaskRoi::~TaskRoi()
{

}

bool TaskRoi::run()
{

    qDebug() << "about to set roi width:" << newRoi.width << "height:" << newRoi.height;

    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>> roidiff;

    rectTransform(oldRoi, newRoi, roidiff.first, roidiff.second);

    Subscription::Lock<cv::Rect, std::pair<std::vector<cv::Rect>,
            std::vector<cv::Rect>>> lock(*sub("dest"));
    lock.swap(newRoi);
    lock.swapMeta(roidiff);
    lock.setVersion(lock.version()+1);

    return true;

}
