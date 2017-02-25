#include "labels_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "subscription_factory.h"
#include "data.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "qtopencv.h"

#include <QVector>

#include "task/labels/task_set_labels.h"
#include "task/labels/task_labels_alter_pixels.h"
#include "task/labels/task_add_label.h"
#include "task/labels/task_labels_icons.h"
#include "task/labels/task_merge_labels.h"
#include "task/labels/task_labels_consolidate.h"

LabelsModel::LabelsModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), iconSize(32, 32)
{
    registerData("labels", {"ROI"});
    registerData("labels.icons", {"labels"});
}

void LabelsModel::delegateTask(QString id, QString parentId)
{

    if (id == "labels") {
        setLabels(lastLabeling, true);
    } else if (id == "labels.icons") {
        computeIcons();
    }

}

void LabelsModel::setImageSize(cv::Size imgSize)
{
    originalImageSize = imgSize;
}

void LabelsModel::setLabels(const cv::Mat1s &labeling)
{
    Labeling vl(labeling, false);
    setLabels(vl, false);
}

void LabelsModel::setLabels(const Labeling &labeling, bool full)
{
    lastLabeling = labeling;
    sendTask(std::make_shared<TaskSetLabels>(labeling, originalImageSize, full));
}

void LabelsModel::alterPixels(const cv::Mat1s &newLabels, const cv::Mat1b &mask)
{
    lastMask = mask.clone(); //looks tricky, but it's the best I can for now
    sendTask(std::make_shared<TaskLabelsAlterPixels>(newLabels, lastMask));
}

void LabelsModel::addLabel()
{
    sendTask(std::make_shared<TaskAddLabel>());
}

void LabelsModel::computeIcons()
{
    sendTask(std::make_shared<TaskLabelsIcons>(iconSize, applyROI));
}

void LabelsModel::setApplyROI(bool applyROI)
{
    this->applyROI = applyROI;
    computeIcons();
}

void LabelsModel::setIconsSize(QSize size)
{
    this->iconSize = size;
    computeIcons();
}

void LabelsModel::mergeLabels(const QVector<int> mlabels)
{
    sendTask(std::make_shared<TaskMergeLabels>(mlabels));
}

void LabelsModel::deleteLabels(const QVector<int> mlabels)
{
    QVector<int> tmp = mlabels;
    tmp.append(0);

    sendTask(std::make_shared<TaskMergeLabels>(tmp));
}

void LabelsModel::consolidateLabels()
{
    sendTask(std::make_shared<TaskLabelsConsolidate>(originalImageSize));
}
