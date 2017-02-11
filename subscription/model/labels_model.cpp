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

LabelsModel::LabelsModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
    registerData("labels", {"image.IMG", "ROI"});

}

void LabelsModel::delegateTask(QString id, QString parentId)
{
    return;

}

void LabelsModel::setLabels(const cv::Mat1s &labeling)
{
    Labeling vl(labeling, false);
    setLabels(vl, false);
}

void LabelsModel::setLabels(const Labeling &labeling, bool full)
{
    std::shared_ptr<Task> task(new TaskSetLabels(labeling, full));
    sendTask(task);
}
