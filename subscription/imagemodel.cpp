#include "imagemodel.h"

#include "task_scheduler.h"
#include "task/task.h"
#include "task/task_image_lim.h"
#include "task/task_norml2tbb.h"
#include "task/task_image_img.h"

ImageModel::ImageModel(bool limitedMode, SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), limitedMode(limitedMode)
{
    registerData("image", {});
    registerData("image.IMG", {"image"});
    registerData("image.NORM", {"image.IMG"});
}

void ImageModel::delegateTask(QString id)
{
    Task* task = nullptr;
    if(id == "image") {
        task = new TaskImageLim(filename, limitedMode);
    } else if(id == "image.IMG") {
        task = new TaskImageIMG(roi, rescaleBands, nBands);
    } else if(id == "image.NORM") {
        task = new TaskNormL2Tbb();
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

void ImageModel::setROI(cv::Rect newROI) {
    roi = newROI;
}
