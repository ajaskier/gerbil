#include "dist_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "data_register.h"
#include "data.h"
#include "lock.h"

#include "task/dist/task_dist_add.h"
#include "task/dist/task_dist_sub.h"

//#include "labeling.h"
#include "qtopencv.h"

#include <QVector>

DistModel::DistModel(TaskScheduler *scheduler, QObject *parent)
    : Model(scheduler, parent)
{
    registerData("dist.tmp.IMG", {"image.IMG", "labels", "ROI"});
    registerData("dist.IMG", {"image.IMG", "labels", "ROI"}); // it needs dist.tmp.IMG too!
}

void DistModel::imageIMGUpdated()
{

    qDebug() << "inside distModel on image.IMG updated";

	int imageMinorVersion = DataRegister::minorVersion("image.IMG");
	int labelsMinorVersion = DataRegister::minorVersion("labels");

    qDebug() << "image minor version" << imageMinorVersion;
    if (imageMinorVersion > 0 || labelsMinorVersion > 0 || directRequest) {
        qDebug() << "non-ROI update";
        addImage(false);
    } else {
        qDebug() << "ROI update";
        addImage(true);
    }
}

void DistModel::fromLabelsUpdate()
{
    bool roiUpdate = !DataRegister::isUpToDate("ROI");
    //if it's the ROI update don't run partial labels update
    //but regular dist calculation
    if (roiUpdate) return;

    if (!DataRegister::isInitialized("dist.IMG")) return;
    qDebug() << "labels partial update!!!";
    labelsUpdateScheduled = true;
    labelsPartialUpdate();
}

void DistModel::fromROIUpdate()
{
    int imageVersion = DataRegister::majorVersion("image.IMG");
    qDebug() << "roi wants me, scheduling subImage";

    subImage(imageVersion);
}

void DistModel::directUpdate()
{
    directRequest = true;

    if (DataRegister::isUpToDate("image.IMG") && !labelsUpdateScheduled) {
        imageIMGUpdated();
    }
}


void DistModel::labelsPartialUpdate()
{
    sendTask<TaskDistSub>("dist.tmp.IMG",
                SourceDeclaration("image.IMG"),
                SourceDeclaration("dist.IMG",
                DataRegister::majorVersion("dist.IMG")),
                illuminant, false, true);

    sendTask<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                SourceDeclaration("dist.tmp.IMG"), illuminant, true, true);

}

void DistModel::delegateTask(QString id, QString parentId)
{
    if (id == "dist.tmp.IMG") return;

    if (parentId == "ROI") {
        fromROIUpdate();
    } else if (parentId == "labels") {
        fromLabelsUpdate();
        return;
    } else if (parentId.isEmpty()) {
        directUpdate();
    }

    if (!imgSub) {
        imgSub = std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency("image.IMG",
                                                SubscriptionType::READ, AccessType::DEFERRED), this,
                                                std::bind(&DistModel::imageIMGUpdated, this)));
    }

    if (!distTmpSub) {
        distTmpSub = std::unique_ptr<Subscription>(DataRegister::subscribe(Dependency("dist.tmp.IMG",
                                                    SubscriptionType::READ, AccessType::DIRECT)));
    }

}

void DistModel::taskFinished(QString id, bool success)
{
    Model::taskFinished(id, success);

    if (id == "taskAddLabelsUpdate") {
        labelsUpdateScheduled = false;
    }

    if (id == "taskAdd" || id == "taskAddArg") {
        distTmpSub.reset(nullptr);
        imgSub.reset(nullptr);
    }
    directRequest = false;

}

void DistModel::addImage(bool withTemp)
{
	if (DataRegister::isInitialized("dist.IMG")) {

        if (withTemp) {
            sendTask<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                    SourceDeclaration("dist.tmp.IMG"), illuminant, true);

        } else {

            sendTask<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                  illuminant, true);
        }

    } else {
        ViewportCtx* ctx = createInitialContext();

        if (withTemp) {
            sendTask<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                        SourceDeclaration("dist.tmp.IMG"),
                                        illuminant, true, false, ctx);
        } else {
            sendTask<TaskDistAdd>("dist.IMG", SourceDeclaration("image.IMG"),
                                        illuminant, true, false, ctx);

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
	if (DataRegister::isInitialized("dist.IMG")) {
        sendTask<TaskDistSub>("dist.tmp.IMG",
		                      SourceDeclaration("image.IMG", version),
                              SourceDeclaration("dist.IMG",
                              DataRegister::majorVersion("dist.IMG")),
                              illuminant, false);
    } else {
        ViewportCtx* ctx = createInitialContext();
        sendTask<TaskDistSub>("dist.tmp.IMG",
                                SourceDeclaration("image.IMG", version),
                                illuminant,
                                false, false, ctx);
    }
}
