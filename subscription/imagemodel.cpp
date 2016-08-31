#include "imagemodel.h"

#include "task_scheduler.h"
#include "task/task.h"
#include "task/task_image_lim.h"
#include "task/task_norml2tbb.h"
#include "task/task_image_img.h"
#include "task/task_image_norm.h"
#include "task/task_image_grad.h"
#include "task/task_image_pca.h"
#include "task/task_gradient_tbb.h"
#include "task/task_pca_tbb.h"
#include "task/task_band.h"

#include <QVector>

ImageModel::ImageModel(bool limitedMode, SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), limitedMode(limitedMode)
{
    registerData("image", {});
    registerData("image.IMG", {"image"});
    registerData("image.NORM", {"image.IMG"});
    registerData("image.GRAD", {"image.IMG"});
    registerData("image.IMGPCA", {"image.IMG"});
    registerData("image.GRADPCA", {"image.GRAD"});

}

void ImageModel::setBandsCount(size_t bands)
{
    QVector<QString> vec = {"IMG", "NORM", "GRAD", "IMGPCA", "GRADPCA"};
    for (QString repr : vec) {
        for(int i = 0; i < bands; i++) {
            registerData("bands."+repr+"."+QString::number(i),
            {"image."+repr});
        }
    }
}

void ImageModel::delegateTask(QString id)
{
    Task* task = nullptr;
    if (id == "image") {
        task = new TaskImageLim(filename, limitedMode);
    } else if (id == "image.IMG") {

        auto range = normalizationRanges[representation::IMG];
        auto mode = normalizationModes[representation::IMG];

        task = new TaskImageIMG(roi, rescaleBands, nBands, mode, range,
                                representation::IMG, true);
    } else if (id == "image.NORM") {

        auto range = normalizationRanges[representation::NORM];
        auto mode = normalizationModes[representation::NORM];

        task = new TaskImageNORM(mode, range, representation::NORM, true);

    } else if (id == "image.GRAD") {

        auto range = normalizationRanges[representation::GRAD];
        auto mode = normalizationModes[representation::GRAD];

        task = new TaskImageGRAD(mode, range, representation::GRAD, true);

    } else if (id == "image.IMGPCA") {

        auto range = normalizationRanges[representation::IMGPCA];
        auto mode = normalizationModes[representation::IMGPCA];

        task = new TaskImagePCA("image.IMG", "image.IMGPCA",
                    mode, range, representation::IMGPCA, true, 10);

    } else if (id == "image.GRADPCA") {

        auto range = normalizationRanges[representation::GRADPCA];
        auto mode = normalizationModes[representation::GRADPCA];

        task = new TaskImagePCA("image.GRAD", "image.GRADPCA",
                                mode, range, representation::GRADPCA, 0);

    } else if (id.startsWith("bands")) {
        auto args = id.split(".");

        representation::t repr;
        if (args[1] == "IMG") repr = representation::IMG;
        else if (args[1] == "NORM") repr = representation::NORM;
        else if (args[1] == "GRAD") repr = representation::GRAD;
        else if (args[1] == "IMGPCA") repr = representation::IMGPCA;
        else if (args[1] == "GRADPCA") repr = representation::GRADPCA;

        task = new TaskBand("image."+args[1], id, args[2].toInt(), repr);
    }
    scheduler->pushTask(task);
}

void ImageModel::setNormalizationParameters(representation::t type,
                                            multi_img::NormMode normMode,
                                            multi_img_base::Range targetRange)
{
    normalizationModes[type] = normMode;
    normalizationRanges[type] = targetRange;

    delegateTask("image."+representation::str(type));
}

void ImageModel::setFilename(QString filename)
{
    this->filename = filename;
}

void ImageModel::spawn(representation::t type, const cv::Rect &newROI, int bands)
{
    if (type == representation::IMG) {
        nBandsOld = nBands;
        oldRoi = roi;
    }
    roi = newROI;
    rescaleBands = bands;

    if (type == representation::IMG) {
        delegateTask("image.IMG");
    }
}

//void ImageModel::computeBand(representation::t type, int dim) {

//}

void ImageModel::setROI(cv::Rect newROI) {
    roi = newROI;

    delegateTask("image.IMG");
}
