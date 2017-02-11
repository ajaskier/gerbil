#ifndef LABELS_MODEL_H
#define LABELS_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"

#include "labeling.h"

class Labels
{
public:
    Labels() {}

    cv::Mat1s fullLabels;
    cv::Mat1s scopedlabels;
    QVector<QColor> colors;
};

class LabelsModel : public Model
{
    Q_OBJECT

public:
    explicit LabelsModel(SubscriptionManager& sm,
                        TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;

    void setLabels(const Labeling &labeling, bool full);
    void setLabels(const cv::Mat1s &labeling);

private:

    bool applyROI;
    QSize iconSize;
};

#endif // LABELS_MODEL_H
