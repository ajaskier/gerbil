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

//#include "labeling.h"
#include "qtopencv.h"


#include <QVector>

FakeModel::FakeModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
    registerData("dist.tmp.IMG", {"image.IMG", "ROI"});
    registerData("dist.IMG", {"image.IMG", "ROI"}); // it needs dist.tmp.FAKE too!
}


void FakeModel::delegateTask(QString id, QString parentId)
{

    if (parentId == "ROI") {
        qDebug() << "ROI WANTS MEEE \n\n";
    } else if (parentId == "image.IMG") {
        qDebug() << "image IMG wants me\n\n";
    }
    if (id == "dist.tmp.IMG") return;


    int roiVersion = DataConditionInformer::version("ROI");
    int imageVersion = DataConditionInformer::version("image.IMG");
    int distVersion = DataConditionInformer::version("dist.IMG");

    ViewportCtx* context = nullptr;
    if (DataConditionInformer::isInitialized("dist.IMG")) {
        std::shared_ptr<Subscription> distSub(
                    SubscriptionFactory::create(Dependency("dist.IMG", SubscriptionType::READ),
                                                SubscriberType::TASK));

        Subscription::Lock<std::vector<BinSet>, ViewportCtx> dist_lock(*distSub);
        context = dist_lock.meta();
    }

    if (!imageVersion || !DataConditionInformer::isInitialized("image.IMG")) {
        qDebug() << "ONLY ADD WITH" << imageVersion+1;
        addImage(imageVersion+1, context, false);
    } else if(imageVersion == roiVersion && distVersion != imageVersion) {
        addImage(imageVersion, context, false);
    } else {
        qDebug() << "SUB WITH IMG VERSION" << imageVersion << "AND ADD WITH" << imageVersion+1;
        subImage(imageVersion, context);
        addImage(imageVersion+1, context, true);
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

    //Task* taskAdd;
    std::shared_ptr<Task> taskAdd;
    if (withTemp) {
        taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.tmp.IMG", id, "dist.IMG", ctx,
                                        /*labels, labelColors,*/ illuminant,
                                        cv::Mat1b(), true));
    } else {
        taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.IMG", id, ctx, /*labels,
                                        labelColors,*/ illuminant, cv::Mat1b(),
                                        true));
    }

    //scheduler->pushTask(taskAdd);
    sendTask(taskAdd);

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
//    Task* taskSub = new TaskDistSub(id, "dist.tmp.IMG", ctx, /*labels,
//                                    labelColors,*/ illuminant, cv::Mat1b(), false);
    std::shared_ptr<Task> taskSub(new TaskDistSub(id, "dist.tmp.IMG", ctx, /*labels,
                                              labelColors,*/ illuminant, cv::Mat1b(), false));


    //scheduler->pushTask(taskSub);
    sendTask(taskSub);
}
