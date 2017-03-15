#include "task_dist_add.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#include "model/labels_model.h"
#include "data_register.h"

#define REUSE_THRESHOLD 0.1

TaskDistAdd::TaskDistAdd(QString destId, SourceDeclaration sourceImgId,
                         std::vector<multi_img::Value> &illuminant, bool apply, bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskAdd", destId, {{"source", sourceImgId}, {"labels", {"labels"}},
                            {"ROI", {"ROI"}}}, illuminant), apply(apply), partialLabelsUpdate(partialLabelsUpdate),
                            args(args)
{
}

TaskDistAdd::TaskDistAdd(QString destId, SourceDeclaration sourceImgId, SourceDeclaration sourceTempId,
                         std::vector<multi_img::Value> &illuminant, bool apply, bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskAdd", destId, {{"source", sourceImgId}, {"temp", sourceTempId},
                        {"labels", {"labels"}}, {"ROI", {"ROI"}}}, illuminant),
                        apply(apply), partialLabelsUpdate(partialLabelsUpdate),
                        args(args)
{
}

TaskDistAdd::~TaskDistAdd()
{

}

std::vector<BinSet> TaskDistAdd::coreExecution(ViewportCtx *args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask)
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    Subscription::Lock<cv::Rect,
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*sub("ROI"));

    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>> roidiff = *(roi_lock.meta());

    bool reuse = !roidiff.second.empty() || partialLabelsUpdate;
    bool keepOldContext;

    if (reuse) {
        keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
                          (fabs(args->minval - source->minval))) &&
            ((fabs(args->maxval) * REUSE_THRESHOLD) >=
             (fabs(args->maxval - source->maxval)));

        if (!keepOldContext) {
            reuse = false;
            if (!partialLabelsUpdate) roidiff.second.clear();
        }
    }

    std::vector<BinSet> result;

    if (reuse && subExists("temp")) {
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> temp_lock(*sub("temp"));
        if(temp_lock()) result = *temp_lock();
    } else if (!partialLabelsUpdate) {
        roidiff.second.clear();
        roidiff.second.push_back(cv::Rect(0, 0, source->width, source->height));
    }

    createBinSets(*source, colors, result);
    updateContext(*source, args);

    std::vector<cv::Rect> diff;
    if (partialLabelsUpdate) {
        diff.push_back( cv::Rect(0, 0, mask.cols, mask.rows) );
    } else {
        diff = roidiff.second;
    }

    expression(false, diff, *source, result, labels, mask, args);
    return result;
}

bool TaskDistAdd::run()
{

    qDebug() << "task dist add running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
    if(!args) args = dest_lock.meta();

    Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
    Labels l = *labels_lock();

    cv::Mat1b mask;
    if (partialLabelsUpdate) {
        LabelsMeta lMeta = *labels_lock.meta();
        mask = lMeta.mask;
    } else {
        mask = cv::Mat1b();
    }

    std::vector<BinSet> result = coreExecution(args, l.scopedlabels, l.colors, mask);

    if (isCancelled()) {
        return false;
    }

    Subscription::Lock<multi_img> source_lock(*sub("source"));

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);
    dest_lock.setVersion(source_lock.version());

    return true;

}
