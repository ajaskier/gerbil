#include "task_dist_add_labels_partial.h"
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

TaskDistAddLabelsPartial::TaskDistAddLabelsPartial(QString destId, SourceDeclaration sourceImgId,
                                                    SourceDeclaration sourceTempId,
                                                    std::vector<multi_img::Value> &illuminant,
                                                    bool apply)
    : TaskDistviewBinsTbb("taskAddLabelsUpdate", destId, {{"source", sourceImgId}, {"temp", sourceTempId},
                            {"labels", {"labels"}}}, illuminant), apply(apply)
{
}

TaskDistAddLabelsPartial::~TaskDistAddLabelsPartial()
{

}

std::vector<BinSet> TaskDistAddLabelsPartial::coreExecution(ViewportCtx *args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask)
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    bool reuse = true;
    bool keepOldContext;


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
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> temp_lock(*sub("temp"));
        if(temp_lock()) result = *temp_lock();
    }

    createBinSets(*source, colors, result);
    updateContext(*source, args);

    std::vector<cv::Rect> diff;
    diff.push_back( cv::Rect(0, 0, mask.cols, mask.rows) );

    expression(false, diff, *source, result, labels, mask, args);

    return result;
}

bool TaskDistAddLabelsPartial::run()
{

    qDebug() << "task dist add running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
    ViewportCtx* args = dest_lock.meta();

    Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
    Labels l = *labels_lock();
    LabelsMeta lMeta = *labels_lock.meta();
    cv::Mat1b mask = lMeta.mask;

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
