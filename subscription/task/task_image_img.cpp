#include "task_image_img.h"
#include "subscription.h"
#include "subscription_factory.h"

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
    createSubscriptions(&scopeImage);
    scopeImage.start();

    qDebug() << "halfway there";

    TaskRescaleTbb rescaleTbb(bands, roiBands, includecache);
    createSubscriptions(&rescaleTbb);
    rescaleTbb.start();
}

void TaskImageIMG::setSubscription(QString id, std::unique_ptr<Subscription> sub)
{
    if (id == "image") sourceSub = std::move(sub);
    else if (id == "image.IMG") targetSub = std::move(sub);
}

void TaskImageIMG::createSubscriptions(Task *task)
{
    auto dependencies = task->getDependencies();
    for(auto& dependency : dependencies) {

        std::unique_ptr<Subscription> s(SubscriptionFactory::create(dependency,
                                                                    SubscriberType::TASK));
        task->setSubscription(dependency.dataId, std::move(s));
    }
}

