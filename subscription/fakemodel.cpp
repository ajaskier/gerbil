#include "fakemodel.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "subscription_factory.h"
#include "data.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "task/dist/task_dist_add.h"
#include "task/dist/task_dist_sub.h"
#include "task/dist/task_dist_add_arg.h"
#include "task/dist/task_dist_sub_arg.h"

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

void FakeModel::imageIMGUpdated()
{
    qDebug() << "inside distModel on image.IMG updated";

    int imageMinorVersion = DataConditionInformer::minorVersion("image.IMG");
    qDebug() << "image minor version" << imageMinorVersion;
    if (imageMinorVersion > 0 || directRequest) {
        qDebug() << "non-ROI update";
        addImage(/*context, */false);
    } else {
        qDebug() << "ROI update";
        addImage(/*context, */true);
    }
}


void FakeModel::delegateTask(QString id, QString parentId)
{
    if (id == "dist.tmp.IMG") return;

    if (parentId == "ROI") {

        int imageVersion = DataConditionInformer::majorVersion("image.IMG");
        qDebug() << "roi wants me, scheduling subImage";
        subImage(imageVersion);


    } else if (parentId.isEmpty()) {

        directRequest = true;

        if (DataConditionInformer::isInitialized("image.IMG")) {
            imageIMGUpdated();
        }
    }

    if (!imgSub) {
        imgSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ),
                                                AccessType::DEFERRED, this,
                                                std::bind(&FakeModel::imageIMGUpdated, this)));
    }

    if (!distTmpSub) {
        distTmpSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("dist.tmp.IMG", SubscriptionType::READ),
                                                                               AccessType::DIRECT));
    }

}

void FakeModel::taskFinished(QString id, bool success)
{
    Model::taskFinished(id, success);

    if (id == "taskAdd" || id == "taskAddArg") {
        distTmpSub.reset(nullptr);
        imgSub.reset(nullptr);
    }
    directRequest = false;

}


void FakeModel::addImage(bool withTemp)
{
    QString id = "image.IMG";
    std::shared_ptr<Task> taskAdd;
    if (DataConditionInformer::isInitialized("dist.IMG")) {

        if (withTemp) {
            taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.IMG", id,
                                            "dist.tmp.IMG",
                                            illuminant,
                                            cv::Mat1b(), true));
        } else {
            taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.IMG", id,
                                            illuminant,
                                            cv::Mat1b(), true));
        }

    } else {
        ViewportCtx* ctx = createInitialContext();

        if (withTemp) {
            taskAdd = std::shared_ptr<Task>(new TaskDistAddArg("dist.IMG", id,
                                            "dist.tmp.IMG", ctx,
                                            illuminant,
                                            cv::Mat1b(), true));
        } else {
            taskAdd = std::shared_ptr<Task>(new TaskDistAddArg("dist.IMG", id,
                                            ctx, illuminant,
                                            cv::Mat1b(), true));
        }

    }
    sendTask(taskAdd);

}

ViewportCtx* FakeModel::createInitialContext()
{
    ViewportCtx *ctx = new ViewportCtx();
    ctx->ignoreLabels = true;
    ctx->nbins = 64;

    ctx->valid = false;
    ctx->reset.fetch_and_store(1);
    ctx->wait.fetch_and_store(1);

    return ctx;
}

void FakeModel::subImage(int version)
{
    QString id = "image.IMG-" + QString::number(version);

    std::shared_ptr<Task> taskSub;
    if (DataConditionInformer::isInitialized("dist.IMG")) {
        taskSub = std::shared_ptr<Task>(new TaskDistSub(id, "dist.tmp.IMG",
                                        illuminant, cv::Mat1b(), false));
    } else {
        ViewportCtx* ctx = createInitialContext();
        taskSub = std::shared_ptr<Task>(new TaskDistSubArg(id, "dist.tmp.IMG",
                                        ctx, illuminant, cv::Mat1b(), false));

    }

    sendTask(taskSub);
}
