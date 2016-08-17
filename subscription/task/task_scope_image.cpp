#include "task_scope_image.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include <QDebug>

TaskScopeImage::TaskScopeImage(cv::Rect roi)
    : Task("image.IMG", {{"image", "source"}}), roi(roi)
{
}

TaskScopeImage::~TaskScopeImage()
{}

bool TaskScopeImage::run()
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));

    multi_img tmp(*source_lock(), roi);

    Subscription::Lock<multi_img> dest_lock(*sub("dest"));
    dest_lock.swap(tmp);

    qDebug() << "scoped finished!";

    return true;
}
