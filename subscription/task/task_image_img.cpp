#include "task_image_img.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

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
    std::shared_ptr<multi_img> scoped(new multi_img());
    TaskScopeImage scopeImage(imgSub, roi, scoped);
    scopeImage.start();

    TaskRescaleTbb rescaleTbb(imgSub, imgIMGSub, scoped, bands, roiBands,
                              includecache);
    rescaleTbb.start();
    qDebug() << "imageIMG finished";
}

void TaskImageIMG::setSubscription(QString id, Subscription *sub)
{
    if(id == "image") imgSub = sub;
    else if(id == "image.IMG") imgIMGSub = sub;
}

void TaskImageIMG::endSubscriptions()
{
    imgSub->end();
    imgIMGSub->end();
}
