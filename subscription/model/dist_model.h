#ifndef DIST_MODEL_H
#define DIST_MODEL_H

#include <QColor>
#include <QVector>

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>
#include "subscription.h"

#include "multi_img.h"
#include "distviewcompute_utils.h"
//class ViewportCtx;

class DistModel : public Model
{
    Q_OBJECT

public:
	explicit DistModel(TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;

    void addImage(bool withTemp = false, bool partialLabelsUpdate = false);
    void subImage(int version, bool partialLabelsUpdate = false);

public slots:
    void taskFinished(QString id, bool success);

private:

    void imageIMGUpdated();
    void fromLabelsUpdate();

    void labelsPartialUpdate();
    void fromROIUpdate();
    void directUpdate();

    ViewportCtx* createInitialContext();

    std::unique_ptr<Subscription> imgSub;
    std::unique_ptr<Subscription> distTmpSub;
    std::unique_ptr<Subscription> labelsSub;

    representation::t type;
    cv::Mat1s labels;
    QVector<QColor> labelColors;

    std::vector<multi_img::Value> illuminant;
    bool ignoreLabels;
    cv::Mat1b highlightMask;
    bool inbetween;


    bool directRequest = false;
    bool labelsUpdateScheduled = false;
};

#endif // DIST_MODEL_H
