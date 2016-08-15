#include "task_image_img.h"
#include "subscription.h"

#include "task_scope_image.h"
#include "task_rescale_tbb.h"
#include <QDebug>

TaskImageIMG::TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                               bool includecache, QObject *parent)
    : Task("image.IMG", parent), roi(roi), bands(bands), roiBands(roiBands),
      includecache(includecache)
{
    dependencies = {Dependency("image.IMG", SubscriptionType::WRITE),
                    Dependency("image", SubscriptionType::READ)};
}

TaskImageIMG::~TaskImageIMG()
{}

void TaskImageIMG::run()
{
    TaskScopeImage scopeImage(roi);
    scopeImage.setSubscription("image", sourceSub);
    scopeImage.setSubscription("image.IMG", targetSub);
    scopeImage.start();

    qDebug() << "halfway there";

    TaskRescaleTbb rescaleTbb(bands, roiBands, includecache);
    rescaleTbb.setSubscription("image", sourceSub);
    rescaleTbb.setSubscription("image.IMG", targetSub);
    rescaleTbb.start();
}

void TaskImageIMG::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if (id == "image") sourceSub = sub;
    else if (id == "image.IMG") targetSub = sub;
}

