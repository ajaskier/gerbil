#include "labels_model.h"

#include "task_scheduler.h"
#include "task/task.h"

#include "subscription_factory.h"
#include "data.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "qtopencv.h"

#include <QVector>

class Labels
{
public:
    Labels() {}

    cv::Mat1s fullLabels;
    cv::Mat1s scopedlabels;
    QVector<QColor> colors;
};

LabelsModel::LabelsModel(SubscriptionManager &sm,
                       TaskScheduler *scheduler, QObject *parent)
    : Model(sm, scheduler, parent)
{
    registerData("labels", {"image.IMG", "ROI"});

}

void LabelsModel::delegateTask(QString id, QString parentId)
{


}
