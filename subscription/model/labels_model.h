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


public slots:

    void setLabels(const Labeling &labeling, bool full);
    void setLabels(const cv::Mat1s &labeling);

    void alterPixels(const cv::Mat1s &newLabels,
                     const cv::Mat1b &mask);

    void addLabel();
    void setApplyROI(bool applyROI);
    void setIconsSize(QSize size);

private:

    void computeIcons();

    bool applyROI;
    QSize iconSize;

    Labeling lastLabeling;
    cv::Mat1b lastMask;
};

#endif // LABELS_MODEL_H
