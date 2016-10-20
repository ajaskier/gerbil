#ifndef DIST_MODEL_H
#define DIST_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>

#include "multi_img.h"

class ViewportCtx;

class DistModel : public Model
{
    Q_OBJECT

public:
    explicit DistModel(SubscriptionManager& sm,
                        TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;
    virtual ~DistModel() {}

public slots:
    void setLabelColors(QVector<QColor> colors);
    void setIlluminant(QVector<multi_img::Value> illum);

    void addImage(int version, ViewportCtx *context, bool withTemp = false);
    void subImage(int version, ViewportCtx *context);


protected:

    representation::t type;
    cv::Mat1s labels;
    QVector<QColor> labelColors;

    std::vector<multi_img::Value> illuminant;
    bool ignoreLabels;
    cv::Mat1b highlightMask;
    bool inbetween;


};

#endif // DIST_MODEL_H
