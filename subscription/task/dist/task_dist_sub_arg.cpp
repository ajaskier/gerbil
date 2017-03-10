#include "task_dist_sub_arg.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#include "model/labels_model.h"

#define REUSE_THRESHOLD 0.1

TaskDistSubArg::TaskDistSubArg(QString destId, SourceDeclaration sourceId, ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant, bool apply)
    : TaskDistSub(destId, sourceId, {"dist.IMG"}, illuminant, /*mask,*/ apply), args(args)
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
