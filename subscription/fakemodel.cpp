#include "fakemodel.h"

#include "task_scheduler.h"
#include "task/task.h"
#include "subscription.h"
#include "subscription_factory.h"
#include "data.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "task/task_dist_add.h"
#include "task/task_dist_sub.h"

#include "labeling.h"
#include "qtopencv.h"


#include <QVector>

FakeModel::FakeModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
  //  registerData("dist.tmp.FAKE", {"image.FAKE"});
  //  registerData("dist.FAKE", {"image.FAKE"}); // it needs dist.tmp.FAKE too!

    registerData("dist.tmp.IMG", {"image.IMG"});
    registerData("dist.IMG", {"image.IMG"}); // it needs dist.tmp.FAKE too!
}


void FakeModel::delegateTask(QString id, QString parentId)
{
//    Task* task = nullptr;
//    scheduler->pushTask(task);

    //if(id == "dist.FAKE") return;
//    if(id == "dist.tmp.FAKE") return;

//    std::shared_ptr<Subscription> roiSub(
//                SubscriptionFactory::create(Dependency("ROI", SubscriptionType::READ),
//                                            SubscriberType::READER));

//    std::shared_ptr<Subscription> imageFakeSub(
//                SubscriptionFactory::create(Dependency("image.FAKE", SubscriptionType::READ),
//                                            SubscriberType::READER));


//    Subscription::Lock<Data> roi_lock(*roiSub);
//    int roiVersion = roi_lock.version();

//    Subscription::Lock<Data> imageFake_lock(*imageFakeSub);
//    int fakeVersion = imageFake_lock.version();

//    if (roiVersion == fakeVersion+1 && fakeVersion > 0) {
//        qDebug() << "versions match";

//        QString imageVersioned = "image.FAKE-" + QString::number(fakeVersion);
//        Task* taskSub = new TaskFakeSub(imageVersioned);
//        scheduler->pushTask(taskSub);


//        QString imageVersionedHigher = "image.FAKE-" + QString::number(fakeVersion+1);
//        Task* taskAdd = new TaskFakeAdd("dist.FAKE", imageVersionedHigher, "dist.tmp.FAKE");
//        scheduler->pushTask(taskAdd);

//    } else {
//        qDebug() << "versions don't match";

//        QString imageVersioned = "image.FAKE-" + QString::number(fakeVersion+1);
//        Task* taskAdd = new TaskFakeAdd("dist.FAKE", imageVersioned);
//        scheduler->pushTask(taskAdd);

//    }

    if (id == "dist.tmp.IMG") return;

    int roiVersion;
    int imageVersion;
    {

        std::shared_ptr<Subscription> roiSub(
                    SubscriptionFactory::create(Dependency("ROI.diff", SubscriptionType::READ),
                                                SubscriberType::READER));

        std::shared_ptr<Subscription> imageSub(
                    SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
                                                SubscriberType::READER));

        Subscription::Lock<
                std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
                > roi_lock(*roiSub);
       roiVersion = roi_lock.version();

        Subscription::Lock<multi_img> image_lock(*imageSub);

        multi_img* img = image_lock();
        labels = cv::Mat1s(img->height, img->width, (short)0);

        QVector<QColor> col = Vec2QColor(Labeling::colors(2, true));
        col[0] = Qt::white;
        labelColors.swap(col);

        imageVersion = image_lock.version();

    }

    ViewportCtx* context = nullptr;
    if (DataConditionInformer::isInitialized("dist.IMG")) {
        std::shared_ptr<Subscription> distSub(
                    SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ),
                                                SubscriberType::READER));

        Subscription::Lock<std::vector<BinSet>, ViewportCtx> dist_lock(*distSub);
        context = dist_lock.meta();
    }

    if (roiVersion == imageVersion + 1 && imageVersion > 0) {
        subImage(imageVersion, context);
        addImage(imageVersion+1, context, true);
    } else if (roiVersion == 1 && imageVersion == 1) {
        addImage(imageVersion, context);
    } else {
        addImage(imageVersion+1, context);
    }

}


void FakeModel::addImage(int version, ViewportCtx* context, bool withTemp)
{
    if (context) ctx = *context;

    ctx.ignoreLabels = true;

    ctx.nbins = 64;

    ctx.valid = false;
    ctx.reset.fetch_and_store(1);
    ctx.wait.fetch_and_store(1);

    QString id = "image.IMG-" + QString::number(version);

    Task* taskAdd;
    if (withTemp) {
        taskAdd = new TaskDistAdd("dist.tmp.IMG", id, "dist.IMG", ctx,
                                        labels, labelColors, illuminant,
                                        cv::Mat1b(), true);
    } else {
        taskAdd = new TaskDistAdd("dist.IMG", id, ctx, labels,
                                        labelColors, illuminant, cv::Mat1b(),
                                        true);
    }

    scheduler->pushTask(taskAdd);

}

void FakeModel::subImage(int version, ViewportCtx* context)
{
    //ViewportCtx ctx;
    if (context) ctx = *context;

    ctx.ignoreLabels = true;
    ctx.nbins = 64;

    ctx.valid = false;
    ctx.reset.fetch_and_store(1);
    ctx.wait.fetch_and_store(1);

    QString id = "image.IMG-" + QString::number(version);
    Task* taskSub = new TaskDistSub(id, "dist.tmp.IMG", ctx, labels,
                                    labelColors, illuminant, cv::Mat1b(), false);

    scheduler->pushTask(taskSub);
}
