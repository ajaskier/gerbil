#include "task_dist_add.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"

#define REUSE_THRESHOLD 0.1

TaskDistAdd::TaskDistAdd(QString destId, QString sourceImgId, ViewportCtx &args,
                         const cv::Mat1s &labels,
                         QVector<QColor> &colors,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistviewBinsTbb("taskAdd", destId, {{sourceImgId, "source"}, {"ROI.diff", "ROI.diff"}},
                      labels, colors, illuminant,
                      mask),//, inplace, apply)
      args(args), /*inplace(inplace),*/ apply(apply)
{
}

TaskDistAdd::TaskDistAdd(QString sourceTempId, QString sourceImgId, QString destId, ViewportCtx &args,
                         const cv::Mat1s &labels,
                         QVector<QColor> &colors,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistviewBinsTbb("taskAdd", destId,
                    {{sourceImgId, "source"}, {sourceTempId, "temp"}, {"ROI.diff", "ROI.diff"}},
                      labels, colors, illuminant,
                      mask),//, inplace, apply)
      args(args), /*inplace(inplace),*/ apply(apply)
{
}

TaskDistAdd::~TaskDistAdd()
{

}

bool TaskDistAdd::run()
{

    qDebug() << "task dist add running";

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    qDebug() << "image width:" << source->width << "image height: "  << source->height;

    Subscription::Lock<
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*sub("ROI.diff"));

    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>* roidiff = roi_lock();

    bool reuse = !roidiff->second.empty();
    bool keepOldContext;

    if (reuse) {
        keepOldContext = ((fabs(args.minval) * REUSE_THRESHOLD) >=
                          (fabs(args.minval - source->minval))) &&
            ((fabs(args.maxval) * REUSE_THRESHOLD) >=
             (fabs(args.maxval - source->maxval)));

        if (!keepOldContext) {
            reuse = false;
            roidiff->second.clear();
        }
    }

    std::vector<BinSet> result;

    if (reuse && subExists("temp")) {
        Subscription::Lock<std::vector<BinSet>, ViewportCtx> temp_lock(*sub("temp"));
        if(temp_lock()) result = *temp_lock();
    } else {
        roidiff->second.push_back(cv::Rect(0, 0, source->width, source->height));
    }

    createBinSets(*source, result);

    //if (!keepOldContext)
        updateContext(*source, args);

    expression(false, roidiff->second, *source, result, args);


    if (isCancelled()) {
        return false;
    }

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
    dest_lock.swap(result);
    dest_lock.swapMeta(args);
    dest_lock.setVersion(source_lock.version());


    return true;

}
