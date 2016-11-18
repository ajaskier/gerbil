#ifndef FAKE_MODEL_H
#define FAKE_MODEL_H

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

class FakeModel : public Model
{
    Q_OBJECT

public:
    explicit FakeModel(SubscriptionManager& sm,
                        TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;

    void addImage(bool withTemp = false);
    void subImage(int version);

public slots:
    void taskFinished(QString id, bool success);

private:

    void imageIMGUpdated();

    ViewportCtx* createInitialContext();

    std::unique_ptr<Subscription> imgSub;
    std::unique_ptr<Subscription> distTmpSub;

    representation::t type;
    cv::Mat1s labels;
    QVector<QColor> labelColors;

    ViewportCtx ctx;

    std::vector<multi_img::Value> illuminant;
    bool ignoreLabels;
    cv::Mat1b highlightMask;
    bool inbetween;


    bool directRequest = false;
};

#endif // FAKE_MODEL_H
