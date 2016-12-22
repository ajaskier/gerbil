#include "task_scope_image.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include <QDebug>

TaskScopeImage::TaskScopeImage()
    : Task("image.IMG", {{"source", {"image"}}, {"ROI", {"ROI"}}})
{
}

TaskScopeImage::~TaskScopeImage()
{}

bool TaskScopeImage::run()
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));

    multi_img tmp(*source_lock(), *roi_lock());
    qDebug() << "image width:" << tmp.width << "image height:" << tmp.height;

    Subscription::Lock<multi_img> dest_lock(*sub("dest"));
    dest_lock.swap(tmp);

    qDebug() << "scoped finished!";

    return true;
}
