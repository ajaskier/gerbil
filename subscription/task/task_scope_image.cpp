#include "task_scope_image.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include <QDebug>

TaskScopeImage::TaskScopeImage(cv::Rect roi, QObject *parent)
    : Task("scopedImage", parent), roi(roi)
{
    dependencies = {Dependency("image", SubscriptionType::READ),
                   Dependency("image.IMG", SubscriptionType::WRITE)};
}

TaskScopeImage::~TaskScopeImage()
{}

void TaskScopeImage::run()
{
    Subscription::Lock<multi_img> source_lock(*source);

    multi_img tmp(source_lock(), roi);

    Subscription::Lock<multi_img> target_lock(*target);
    target_lock.swap(tmp);

    qDebug() << "scoped finished!";
}

void TaskScopeImage::setSubscription(QString id, std::unique_ptr<Subscription> sub)
{
    if (id == "image") source = std::move(sub);
    else if (id == "image.IMG") target = std::move(sub);
}
