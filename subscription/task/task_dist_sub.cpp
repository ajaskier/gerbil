#include "task_dist_sub.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"

#define REUSE_THRESHOLD 0.1

TaskDistSub::TaskDistSub(QString sourceId, QString destId, ViewportCtx &args,
                         const cv::Mat1s &labels,
                         QVector<QColor> &colors,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistviewBinsTbb("taskSub", destId, {{sourceId, "source"}, {"ROI.diff", "ROI.diff"}},
                      labels, colors, illuminant,
                      mask),//, inplace, apply)
      args(args), /*inplace(inplace),*/ apply(apply)
{
}

TaskDistSub::~TaskDistSub()
{

}

bool TaskDistSub::run()
{
    qDebug() << "task dist sub running";

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
    Subscription::Lock<
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*sub("ROI.diff"));

    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>* roidiff = roi_lock();

    bool reuse = !roidiff->first.empty();
    bool keepOldContext = false;

    if (reuse) {
        keepOldContext = ((fabs(args.minval) * REUSE_THRESHOLD) >=
                          (fabs(args.minval - source->minval))) &&
            ((fabs(args.maxval) * REUSE_THRESHOLD) >=
             (fabs(args.maxval - source->maxval)));

        if (!keepOldContext) {
            reuse = false;
            roidiff->first.clear();
        }
    }

    std::vector<BinSet> result;

    createBinSets(*source, result);

    if (!keepOldContext)
        updateContext(*source, args);

    expression(true, roidiff->first, *source, result, args);

    if (isCancelled()) {
        return false;
    }

    dest_lock.swap(result);
    dest_lock.swapMeta(args);

    return true;

}
