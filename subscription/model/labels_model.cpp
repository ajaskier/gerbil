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

LabelsModel::LabelsModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent), iconSize(32, 32)
{
    registerData("labels", {"image.IMG", "ROI"});
    registerData("labels.icons", {"labels"});
}

void LabelsModel::delegateTask(QString id, QString parentId)
{

    if (id == "labels") {
        setLabels(lastLabeling, true);
    } else if (id == "labels.icons") {
        std::shared_ptr<Task> task(new TaskLabelsIcons(iconSize, true));
        sendTask(task);
    }
 //   return;

}

void LabelsModel::setLabels(const cv::Mat1s &labeling)
{
    Labeling vl(labeling, false);
    setLabels(vl, false);
}

void LabelsModel::setLabels(const Labeling &labeling, bool full)
{
    lastLabeling = labeling;
    std::shared_ptr<Task> task(new TaskSetLabels(labeling, full));
    sendTask(task);
}

void LabelsModel::alterPixels(const cv::Mat1s &newLabels, const cv::Mat1b &mask)
{
    lastMask = mask.clone(); //looks tricky, but it's the best I can for now
    std::shared_ptr<Task> task(new TaskLabelsAlterPixels(newLabels, lastMask));
    sendTask(task);
}

void LabelsModel::addLabel()
{
    std::shared_ptr<Task> task(new TaskAddLabel());
    sendTask(task);
}
