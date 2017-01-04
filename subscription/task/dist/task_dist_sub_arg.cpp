#include "task_dist_sub_arg.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD 0.1

TaskDistSubArg::TaskDistSubArg(QString destId, SourceDeclaration sourceId, ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistSub(destId, sourceId, {""}, illuminant, mask, apply), args(args)
//    : TaskDistviewBinsTbb("taskSubArg", destId, {{"source", sourceId}, {"ROI", {"ROI"}}},
//                      illuminant, mask), args(args), apply(apply)
{
}

TaskDistSubArg::~TaskDistSubArg()
{

}

bool TaskDistSubArg::run()
{
    qDebug() << "task dist sub running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    std::vector<BinSet> result = coreExecution(args);

    if (isCancelled() || result.empty()) {
        return false;
    }

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);

    return true;

}
