#include "imagemodel.h"

#include "task_scheduler.h"
#include "task/task.h"
#include "task/task_image_lim.h"
#include "task/task_norml2tbb.h"
#include "task/task_image_img.h"
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


}

void ImageModel::setBandsCount(size_t bands)
{
    QVector<QString> vec = {"IMG", "NORM", "GRAD", "IMGPCA"};
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
        task = new TaskImageIMG(roi, rescaleBands, nBands);
    } else if (id == "image.NORM") {
        task = new TaskNormL2Tbb();
    } else if (id == "image.GRAD") {
        task = new TaskGradientTbb();
    } else if (id == "image.IMGPCA") {
        task = new TaskPcaTbb(10);
    } else if (id.startsWith("bands")) {
        auto args = id.split(".");

        representation::t repr;
        if (args[1] == "IMG") repr = representation::t::IMG;
        else if (args[1] == "NORM") repr = representation::t::NORM;
        else if (args[1] == "GRAD") repr = representation::t::GRAD;
        else if (args[1] == "IMGPCA") repr = representation::t::IMGPCA;

        task = new TaskBand("image."+args[1], id, args[2].toInt(), repr);
    }
    scheduler->pushTask(task);
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

void ImageModel::computeBand(representation::t type, int dim) {

}

void ImageModel::setROI(cv::Rect newROI) {
    roi = newROI;
}
