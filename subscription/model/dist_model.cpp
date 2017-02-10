#include "dist_model.h"

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

DistModel::DistModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
    registerData("dist.tmp.IMG", {"image.IMG", "ROI"});
    registerData("dist.IMG", {"image.IMG", "ROI"}); // it needs dist.tmp.IMG too!

}

void DistModel::imageIMGUpdated()
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


void DistModel::delegateTask(QString id, QString parentId)
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
        imgSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("image.IMG", SubscriptionType::READ,
                                                                                      AccessType::DEFERRED), this,
                                                std::bind(&DistModel::imageIMGUpdated, this)));
    }

    if (!distTmpSub) {
        distTmpSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("dist.tmp.IMG", SubscriptionType::READ,
                                                                               AccessType::DIRECT)));
    }

}

void DistModel::taskFinished(QString id, bool success)
{
    Model::taskFinished(id, success);

    if (id == "taskAdd" || id == "taskAddArg") {
        distTmpSub.reset(nullptr);
        imgSub.reset(nullptr);
    }
    directRequest = false;

}


void DistModel::addImage(bool withTemp)
{
    std::shared_ptr<Task> taskAdd;
    if (DataConditionInformer::isInitialized("dist.IMG")) {

        if (withTemp) {
            taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.IMG", {"image.IMG"},
                                            {"dist.tmp.IMG"},
                                            illuminant,
                                            cv::Mat1b(), true));
        } else {
            taskAdd = std::shared_ptr<Task>(new TaskDistAdd("dist.IMG", {"image.IMG"},
                                            illuminant,
                                            cv::Mat1b(), true));
        }

    } else {
        ViewportCtx* ctx = createInitialContext();

        if (withTemp) {
            taskAdd = std::shared_ptr<Task>(new TaskDistAddArg("dist.IMG", {"image.IMG"},
                                            {"dist.tmp.IMG"}, ctx,
                                            illuminant,
                                            cv::Mat1b(), true));
        } else {
            taskAdd = std::shared_ptr<Task>(new TaskDistAddArg("dist.IMG", {"image.IMG"},
                                            ctx, illuminant,
                                            cv::Mat1b(), true));
        }

    }
    sendTask(taskAdd);
}

ViewportCtx* DistModel::createInitialContext()
{
    ViewportCtx *ctx = new ViewportCtx();
    ctx->ignoreLabels = true;
    ctx->nbins = 64;

    ctx->valid = false;
    ctx->reset.fetch_and_store(1);
    ctx->wait.fetch_and_store(1);

    return ctx;
}

void DistModel::subImage(int version)
{

    std::shared_ptr<Task> taskSub;
    if (DataConditionInformer::isInitialized("dist.IMG")) {
        taskSub = std::shared_ptr<Task>(new TaskDistSub("dist.tmp.IMG", {"image.IMG", version},
                                        {"dist.IMG", AccessType::FORCED},
                                        illuminant, cv::Mat1b(), false));
    } else {
        ViewportCtx* ctx = createInitialContext();
        taskSub = std::shared_ptr<Task>(new TaskDistSubArg("dist.tmp.IMG", {"image.IMG", version},
                                        ctx, illuminant, cv::Mat1b(), false));

    }

    sendTask(taskSub);
}
