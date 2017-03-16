#include "task_dist_sub.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#include "model/labels_model.h"
#include "model/img_model.h"
#include "data_register.h"

#define REUSE_THRESHOLD 0.1

TaskDistSub::TaskDistSub(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                         std::vector<multi_img::Value> &illuminant, bool apply, bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskSub", destId, {{"source", sourceId},
                            {"sourceDist", sourceDistId}, {"labels", {"labels"}},
                            {"ROI", {"ROI"}}}, illuminant), apply(apply),
                            partialLabelsUpdate(partialLabelsUpdate), args(args)
{
}

TaskDistSub::TaskDistSub(QString destId, SourceDeclaration sourceId,
                        std::vector<multi_img::Value> &illuminant,
                        bool apply, bool partialLabelsUpdate,
                        ViewportCtx* args)
    : TaskDistviewBinsTbb("taskSub", destId, {{"source", sourceId},
                            {"labels", {"labels"}}, {"ROI", {"ROI"}}}, illuminant),
                            apply(apply), partialLabelsUpdate(partialLabelsUpdate), args(args)
{
}

TaskDistSub::~TaskDistSub()
{}

std::vector<cv::Rect> TaskDistSub::getDiff(cv::Mat1b& mask)
{
    Subscription::Lock<cv::Rect, ROIMeta> roi_lock(*sub("ROI"));
    ROIMeta roimeta = *(roi_lock.meta());

    std::vector<cv::Rect> diff;
    if (partialLabelsUpdate) {
        diff.push_back(cv::Rect(0, 0, mask.cols, mask.rows));
    } else if(roimeta.profitable) {
        diff = roimeta.sub;
    }

    return diff;
}

std::vector<BinSet> TaskDistSub::coreExecution(ViewportCtx* args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask,
                                               multi_img& img, std::vector<BinSet>* reuseDist)
{
    auto diff = getDiff(mask);

    bool reuse = !diff.empty();
    bool keepOldContext = false;

    if (reuse) {
        keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
                          (fabs(args->minval - img.minval))) &&
            ((fabs(args->maxval) * REUSE_THRESHOLD) >=
             (fabs(args->maxval - img.maxval)));

        if (!keepOldContext) {
            reuse = false;
        }
    }

    if (diff.empty()) {
        return std::vector<BinSet>();
    }

    std::vector<BinSet> result;
    if (reuse && reuseDist) {
        result = *reuseDist;
    }

    createBinSets(img, colors, result);

    if (!keepOldContext)
        updateContext(img, args);

    expression(true, diff, img, result, labels, mask, args);

    return result;
}

bool TaskDistSub::run()
{

    std::vector<BinSet> *reuseDist = nullptr;
    if (subExists("sourceDist")) {
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> sourceDist_lock(*sub("sourceDist"));
        if(sourceDist_lock()) reuseDist = sourceDist_lock();

        if(!args) {
            args = sourceDist_lock.meta();
        }
    }

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
    Labels l = *labels_lock();

    cv::Mat1b mask;
    cv::Mat1s labels;
    if (partialLabelsUpdate) {
        LabelsMeta lMeta = *labels_lock.meta();
        mask = lMeta.mask;
        labels = lMeta.oldLabels;
    } else {
        mask = cv::Mat1b();
        labels = l.scopedlabels;
    }

    std::vector<BinSet> result = coreExecution(args, labels, l.colors,
                                                mask, *source, reuseDist);


    if (isCancelled() || result.empty()) {
        return false;
    }

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);

    return true;

}
