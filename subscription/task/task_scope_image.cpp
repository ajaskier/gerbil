#include "task_scope_image.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include <QDebug>

TaskScopeImage::TaskScopeImage(Subscription *imgSub, cv::Rect roi,
                               std::shared_ptr<multi_img> target,
                               QObject *parent)
    : Task("scopedImage", parent), imgSub(imgSub), roi(roi), target(target)
{}

TaskScopeImage::~TaskScopeImage()
{}

void TaskScopeImage::run()
{
    Subscription::Lock<multi_img> img_lock(imgSub);
    *target = multi_img(img_lock(), roi);
    qDebug() << "scoped finished!";
}

void TaskScopeImage::setSubscription(QString id, Subscription *sub)
{}

void TaskScopeImage::endSubscriptions()
{}
