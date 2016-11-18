#include "task_dist_add_arg.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD 0.1

TaskDistAddArg::TaskDistAddArg(QString destId, QString sourceImgId, ViewportCtx *args,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistviewBinsTbb("taskAddArg", destId, {{sourceImgId, "source"}, {"ROI", "ROI"}},
                      illuminant, mask), args(args), apply(apply)
{
}

TaskDistAddArg::TaskDistAddArg(QString destId, QString sourceImgId, QString sourceTempId,
                         ViewportCtx* args,
                         std::vector<multi_img::Value> &illuminant, const cv::Mat1b &mask, bool apply)
    : TaskDistviewBinsTbb("taskAddArg", destId,
                    {{sourceImgId, "source"}, {sourceTempId, "temp"}, {"ROI", "ROI"}},
                      illuminant, mask), args(args), apply(apply)
{
}

TaskDistAddArg::~TaskDistAddArg()
{

}

bool TaskDistAddArg::run()
{

    qDebug() << "task dist add running";

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* source = source_lock();

    /* ------ TEMP */
    labels = cv::Mat1s(source->height, source->width, (short)0);
    QVector<QColor> col = Vec2QColor(Labeling::colors(2, true));
    col[0] = Qt::white;
    colors.swap(col);
    /* TEMP ------ */

    qDebug() << "image width:" << source->width << "image height: "  << source->height;

    Subscription::Lock<
            cv::Rect,
            std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>
            > roi_lock(*sub("ROI"));

    std::pair<std::vector<cv::Rect>, std::vector<cv::Rect>>* roidiff = roi_lock.meta();

    bool reuse = !roidiff->second.empty();
    bool keepOldContext;

    Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

    if (reuse) {
        keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
                          (fabs(args->minval - source->minval))) &&
            ((fabs(args->maxval) * REUSE_THRESHOLD) >=
             (fabs(args->maxval - source->maxval)));

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


    dest_lock.swap(result);
    dest_lock.swapMeta(*args);
    dest_lock.setVersion(source_lock.version());


    return true;

}
