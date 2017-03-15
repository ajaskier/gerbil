#include "task_dist_sub_labels_partial.h"
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

TaskDistSubLabelsPartial::TaskDistSubLabelsPartial(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                                                    std::vector<multi_img::Value> &illuminant, bool apply)
    : TaskDistviewBinsTbb("taskSubLabelsUpdate", destId, {{"source", sourceId}, {"sourceDist", sourceDistId},
                            {"labels", {"labels"}}}, illuminant), apply(apply)
{
}

TaskDistSubLabelsPartial::~TaskDistSubLabelsPartial()
{

}

std::vector<BinSet> TaskDistSubLabelsPartial::coreExecution(ViewportCtx* args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask)
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();


    bool reuse = true;
    bool keepOldContext = false;

    if (reuse) {
        keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
                          (fabs(args->minval - source->minval))) &&
            ((fabs(args->maxval) * REUSE_THRESHOLD) >=
             (fabs(args->maxval - source->maxval)));

        if (!keepOldContext) {
            reuse = false;
        }
    }

    std::vector<BinSet> result;

    if (reuse) {
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> dist_lock(*sub("sourceDist"));
        if(dist_lock()) result = *dist_lock();
    }

    createBinSets(*source, colors, result);

    if (!keepOldContext)
        updateContext(*source, args);

    std::vector<cv::Rect> diff;
    diff.push_back(cv::Rect(0, 0, mask.cols, mask.rows));
    expression(true, diff, *source, result, labels, mask, args);

    return result;
}

bool TaskDistSubLabelsPartial::run()
{
    qDebug() << "task dist sub running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> sourceDist_lock(*sub("sourceDist"));
    ViewportCtx* args = sourceDist_lock.meta();

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
    Labels l = *labels_lock();
    LabelsMeta lMeta = *labels_lock.meta();

    std::vector<BinSet> result = coreExecution(args, lMeta.oldLabels, l.colors, lMeta.mask);

    if (isCancelled() || result.empty()) {
        return false;
    }

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);

    return true;

}
