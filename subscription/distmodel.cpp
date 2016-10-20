#include "distmodel.h"

#include "task_scheduler.h"
#include "task/task.h"
#include "subscription.h"
#include "subscription_factory.h"
#include "data.h"
#include "lock.h"

#include "task/task_dist_add.h"
#include "task/task_dist_sub.h"
#include "distviewcompute_utils.h"

#include <QVector>


DistModel::DistModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
    registerData("dist.tmp.IMG", {"image.IMG"});
    registerData("dist.IMG", {"image.IMG"}); // it needs dist.tmp.FAKE too!
}


void DistModel::delegateTask(QString id, QString parentId)
{

    if (id == "dist.tmp.IMG") return;

    std::shared_ptr<Subscription> roiSub(
                SubscriptionFactory::create(Dependency("ROI.diff", SubscriptionType::READ),
                                            SubscriberType::READER));

    std::shared_ptr<Subscription> imageSub(
                SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
                                            SubscriberType::READER));

    std::shared_ptr<Subscription> distSub(
                SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ),
                                            SubscriberType::READER));


    Subscription::Lock<
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*roiSub);
    int roiVersion = roi_lock.version();

    Subscription::Lock<multi_img> image_lock(*imageSub);
    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dist_lock(*distSub);

    int imageVersion = image_lock.version();

    if (roiVersion == imageVersion + 1 && imageVersion > 0) {
        subImage(imageVersion, dist_lock.meta());
        addImage(imageVersion+1, dist_lock.meta(), true);

    } else {
        addImage(imageVersion+1, dist_lock.meta());
    }

}

void DistModel::setLabelColors(QVector<QColor> colors)
{
    labelColors = colors;
}

void DistModel::setIlluminant(QVector<multi_img_base::Value> illum)
{
    illuminant = illum.toStdVector();
}

void DistModel::addImage(int version, ViewportCtx* context, bool withTemp)
{
    ViewportCtx ctx;
    if (context) ctx = *context;

    ctx.valid = false;
    ctx.reset.fetch_and_store(1);
    ctx.wait.fetch_and_store(1);

    QString id = "image.IMG-" + QString::number(version);

    Task* taskAdd;
    if (withTemp) {
        taskAdd = new TaskDistAdd(id, "dist.IMG", "dist.tmp.IMG", ctx,
                                        labels, labelColors, illuminant,
                                        cv::Mat1b(), true);
    } else {
        taskAdd = new TaskDistAdd(id, "dist.IMG", ctx, labels,
                                        labelColors, illuminant, cv::Mat1b(),
                                        true);
    }

    scheduler->pushTask(taskAdd);

}

void DistModel::subImage(int version, ViewportCtx* context)
{
    ViewportCtx ctx;
    if (context) ctx = *context;

    QString id = "image.IMG-" + QString::number(version);
    Task* taskSub = new TaskDistSub(id, "dist.tmp.IMG", ctx, labels,
                                    labelColors, illuminant, cv::Mat1b(), false);

    scheduler->pushTask(taskSub);
}



