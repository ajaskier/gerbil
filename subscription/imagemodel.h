#ifndef IMAGE_MODEL_H
#define IMAGE_MODEL_H

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>

class ImageModel : public Model
{
    Q_OBJECT

public:
    explicit ImageModel(bool limitedMode, SubscriptionManager& sm,
                        TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id) override;
    void spawn(representation::t type, const cv::Rect &newROI, int bands);
    void computeBand(representation::t type, int dim);
    void setBandsCount(size_t bands);

    void setFilename(QString filename);
    void setROI(cv::Rect newROI);

private:
    bool limitedMode;
    QString filename;

    cv::Rect roi;
    cv::Rect oldRoi;

    size_t nBands;
    size_t nBandsOld;
    size_t rescaleBands;

};

#endif // IMAGE_MODEL_H
