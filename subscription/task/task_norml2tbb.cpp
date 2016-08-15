#include "task_norml2tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>


TaskNormL2Tbb::TaskNormL2Tbb(QObject *parent)
    : Task("normL2Tbb", parent)
{
    dependencies = {Dependency("image.NORM", SubscriptionType::WRITE),
                    Dependency("image.IMG", SubscriptionType::READ)};
}

TaskNormL2Tbb::~TaskNormL2Tbb()
{

}

void TaskNormL2Tbb::run()
{
//    Subscription::Lock<multi_img> img_lock(imgIMGSub);
//    multi_img& img = img_lock();




}

void TaskNormL2Tbb::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if(id == "image.NORM") imgNORMSub = sub;
    else if(id == "image.IMG") imgIMGSub = sub;
}
