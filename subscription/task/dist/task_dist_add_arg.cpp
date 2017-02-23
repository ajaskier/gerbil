#include "task_dist_add_arg.h"
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

TaskDistAddArg::TaskDistAddArg(QString destId, SourceDeclaration sourceImgId, ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistAdd(destId, sourceImgId, illuminant, mask, apply), args(args)
//    : TaskDistviewBinsTbb("taskAddArg", destId, {{"source", sourceImgId}, {"ROI", {"ROI"}}},
//                      illuminant, mask), args(args), apply(apply)
{
}

TaskDistAddArg::TaskDistAddArg(QString destId, SourceDeclaration sourceImgId, SourceDeclaration sourceTempId,
                         ViewportCtx* args,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistAdd(destId, sourceImgId, sourceTempId, illuminant, mask, apply), args(args)
//    : TaskDistviewBinsTbb("taskAddArg", destId,
//                    {{"source", sourceImgId}, {"temp", sourceTempId}, {"ROI", {"ROI"}}},
//                      illuminant, mask), args(args), apply(apply)
{
}

TaskDistAddArg::~TaskDistAddArg()
{

}

bool TaskDistAddArg::run()
{

    qDebug() << "task dist add running";

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    Subscription::Lock<Labels> labels_lock(*sub("labels"));
    Labels l = *labels_lock();

    std::vector<BinSet> result = coreExecution(args, l.scopedlabels, l.colors);

    if (isCancelled()) {
        return false;
    }

    Subscription::Lock<multi_img> source_lock(*sub("source"));

    dest_lock.swap(result);
    dest_lock.swapMeta(*args);
    dest_lock.setVersion(source_lock.version());


    return true;

}
