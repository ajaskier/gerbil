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
#include "data_register.h"

#define REUSE_THRESHOLD 0.1

TaskDistSub::TaskDistSub(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                         std::vector<multi_img::Value> &illuminant, bool apply)
    : TaskDistviewBinsTbb("taskSubDest", destId,
                        {{"source", sourceId}, {"sourceDist", sourceDistId}, {"labels", {"labels"}}, {"ROI", {"ROI"}}},
                        illuminant/*, mask*/), apply(apply)
{
}

TaskDistSub::~TaskDistSub()
{

}

std::vector<BinSet> TaskDistSub::coreExecution(ViewportCtx* args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask)
{
    Subscription::Lock<
            cv::Rect,
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*sub("ROI"));
    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>* roidiff = roi_lock.meta();
    if (roidiff->first.empty()) {
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
        std::vector<BinSet> fakeResult;
        dest_lock.swap(fakeResult);

        return std::vector<BinSet>();
    }

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

//    /* ------ TEMP */
//    labels = cv::Mat1s(source->height, source->width, (short)0);
//    QVector<QColor> col = Vec2QColor(Labeling::colors(2, true));
//    col[0] = Qt::white;
//    colors.swap(col);
//    /* TEMP ------ */

    bool reuse = !roidiff->first.empty();
    bool keepOldContext = false;

    if (reuse) {
        keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
                          (fabs(args->minval - source->minval))) &&
            ((fabs(args->maxval) * REUSE_THRESHOLD) >=
             (fabs(args->maxval - source->maxval)));

        if (!keepOldContext) {
            reuse = false;
            roidiff->first.clear();
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
    if (DataRegister::minorVersion("labels") > 1) {
        diff.push_back(cv::Rect(0,0,mask.cols,mask.rows));
    } else {
        diff = roidiff->first;
    }

    expression(true, diff, *source, result, labels, mask, args);

    return result;
}

bool TaskDistSub::run()
{
    qDebug() << "task dist sub running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> sourceDist_lock(*sub("sourceDist"));
    ViewportCtx* args = sourceDist_lock.meta();

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
    Labels l = *labels_lock();
    cv::Mat1b mask = cv::Mat1b();

    std::vector<BinSet> result = coreExecution(args, l.scopedlabels, l.colors, mask);


    if (isCancelled() || result.empty()) {
        return false;
    }

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);

    return true;

}
