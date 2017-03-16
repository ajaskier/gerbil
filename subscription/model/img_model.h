#ifndef IMG_MODEL_H
#define IMG_MODEL_H

#include <model/representation.h>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <model/model.h>

#include "multi_img.h"


class ROIMeta
{
public:
    ROIMeta() {}

    std::vector<cv::Rect> sub;
    std::vector<cv::Rect> add;
    bool profitable;

};

class ImgModel : public Model
{
    Q_OBJECT

public:
	explicit ImgModel(bool limitedMode,
	                  TaskScheduler* scheduler, QObject *parent = nullptr);

    virtual void delegateTask(QString id, QString parentId = "") override;
    void spawn(representation::t type, const cv::Rect &newROI, int bands);
    //void computeBand(representation::t type, int dim);
    void setBandsCount(size_t bands);
    void setNormalizationParameters(representation::t type,
                                    multi_img::NormMode normMode,
                                    multi_img_base::Range targetRange);

    void setFilename(QString filename);
    void setROI(cv::Rect newROI_arg);

    void runImg();

private:
    bool limitedMode;
    QString filename;

    cv::Rect newRoi;
    cv::Rect roi;

    size_t nBands;
    size_t nBandsOld;
    size_t rescaleBands;


    std::map<representation::t, multi_img::NormMode> normalizationModes;
    std::map<representation::t, multi_img_base::Range> normalizationRanges;

    int imgFakeCounter = 0;

};

#endif // IMG_MODEL_H
