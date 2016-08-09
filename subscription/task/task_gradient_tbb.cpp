#include "task_gradient_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"

TaskGradientTbb::TaskGradientTbb(bool includecache, QObject *parent)
    : Task("image.GRAD", parent), includecache(includecache)
{
    dependencies = {Dependency("image.IMG", SubscriptionType::READ),
                    Dependency("image.GRAD", SubscriptionType::WRITE)};
}

TaskGradientTbb::~TaskGradientTbb()
{

}

void TaskGradientTbb::run()
{

}

void TaskGradientTbb::setSubscription(QString id, std::unique_ptr<Subscription> sub)
{
    if(id == "image.IMG") imgIMGSub = std::move(sub);
    else if(id == "image.GRAD") imgGRADSub = std::move(sub);
}
