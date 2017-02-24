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
    registerData("dist.tmp.IMG", {"image.IMG", "labels", "ROI"});
    registerData("dist.IMG", {"image.IMG", "labels", "ROI"}); // it needs dist.tmp.IMG too!

}

void DistModel::imageIMGUpdated()
{
    qDebug() << "inside distModel on image.IMG updated";

    int imageMinorVersion = DataConditionInformer::minorVersion("image.IMG");
    int labelsMinorVersion = DataConditionInformer::minorVersion("labels");
    qDebug() << "image minor version" << imageMinorVersion;
    if (imageMinorVersion > 0 || labelsMinorVersion > 0 || directRequest) {
        qDebug() << "non-ROI update";
        addImage(false);
    } else {
        qDebug() << "ROI update";
        addImage(true);
    }
}

// ######### THIS SHOULD BE POTENTIALLY IMPLEMENTED
//void DistModel::labelsUpdated()
//{
//    qDebug() << "inside distModel on labels updated";
//}


void DistModel::delegateTask(QString id, QString parentId)
{
    if (id == "dist.tmp.IMG") return;

    if (parentId == "ROI") {

        int imageVersion = DataConditionInformer::majorVersion("image.IMG");
        qDebug() << "roi wants me, scheduling subImage";
        subImage(imageVersion);


    } else if (parentId.isEmpty()) {

        directRequest = true;

        //if (DataConditionInformer::isUpToDate("labels")) {
            //labelsUpdated();
        //} else
        if (DataConditionInformer::isUpToDate("image.IMG")) {
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

//    if (!labelsSub) {
//        labelsSub = std::unique_ptr<Subscription>(SubscriptionFactory::create(Dependency("labels", SubscriptionType::READ,
//                                                                                         AccessType::DIRECT), this,
//                                                                              std::bind(&DistModel::labelsUpdated, this)));
//    }

}

void DistModel::taskFinished(QString id, bool success)
{
    Model::taskFinished(id, success);

    if (id == "taskAdd" || id == "taskAddArg") {
        distTmpSub.reset(nullptr);
        imgSub.reset(nullptr);
        //labelsSub.reset(nullptr);

    }
    directRequest = false;

}

void DistModel::addImage(bool withTemp)
{
    if (DataConditionInformer::isInitialized("dist.IMG")) {

        if (withTemp) {
            sendTask(std::make_shared<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                                    SourceDeclaration("dist.tmp.IMG"), illuminant,
                                                    cv::Mat1b(), true));

        } else {                            
            sendTask(std::make_shared<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                                    illuminant, cv::Mat1b(), true)
                    );
        }

    } else {
        ViewportCtx* ctx = createInitialContext();

        if (withTemp) {
            sendTask(std::make_shared<TaskDistAddArg>("dist.IMG", SourceDeclaration("image.IMG"),
                                                        SourceDeclaration("dist.tmp.IMG"), ctx,
                                                        illuminant, cv::Mat1b(),
                                                        true));
        } else {
            sendTask(std::make_shared<TaskDistAddArg>("dist.IMG", SourceDeclaration("image.IMG"),
                                                        ctx, illuminant, cv::Mat1b(),
                                                        true));
        }

    }
}

ViewportCtx* DistModel::createInitialContext()
{
    ViewportCtx *ctx = new ViewportCtx();
    ctx->ignoreLabels = false;
    ctx->nbins = 64;

    ctx->valid = false;
    ctx->reset.fetch_and_store(1);
    ctx->wait.fetch_and_store(1);

    return ctx;
}

void DistModel::subImage(int version)
{
    if (DataConditionInformer::isInitialized("dist.IMG")) {
        sendTask(std::make_shared<TaskDistSub>("dist.img.IMG",
                                                SourceDeclaration("image.IMG", version),
                                                SourceDeclaration("dist.IMG", AccessType::FORCED),
                                                illuminant, cv::Mat1b(), false));
    } else {
        ViewportCtx* ctx = createInitialContext();
        sendTask(std::make_shared<TaskDistSubArg>("dist.tmp.IMG",
                                                    SourceDeclaration("image.IMG", version),
                                                    ctx, illuminant, cv::Mat1b(),
                                                    false));
    }
}
