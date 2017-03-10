#include "task_image_img.h"
#include "subscription.h"
#include "lock.h"

#include "task_scope_image.h"
#include "task_rescale_tbb.h"
#include "task_normrange_tbb.h"
#include <QDebug>

TaskImageIMG::TaskImageIMG(size_t bands, size_t roiBands,
                           multi_img::NormMode normMode,
                           multi_img_base::Range normRange,
                           representation::t type, bool update, bool includecache)
    : Task("image.IMG", {{"source", {"image"}}, {"ROI", {"ROI"}}}), bands(bands),
      roiBands(roiBands), includecache(includecache), normMode(normMode),
      normRange(normRange), type(type), update(update)
{

}

TaskImageIMG::~TaskImageIMG()
{}

bool TaskImageIMG::run()
{
    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;
    auto roiId = sub("ROI")->getDependency().dataId;

    {
        Subscription::Lock<multi_img> dest_lock(*sub("dest"));
        Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
        dest_lock.setVersion(roi_lock.version());
    }

    TaskScopeImage scopeImage;
    scopeImage.setSubscription(sourceId, sub("source"));
    scopeImage.setSubscription(destId, sub("dest"));
    scopeImage.setSubscription(roiId, sub("ROI"));
    auto success = scopeImage.start();
    if(!success) return success;

    qDebug() << "halfway there";

    TaskRescaleTbb rescaleTbb(bands, roiBands, includecache);
    rescaleTbb.setSubscription(sourceId, sub("source"));
    rescaleTbb.setSubscription(destId, sub("dest"));
    success = rescaleTbb.start();
    if(!success) return success;

    TaskNormRangeTbb normRangeTbb("image.IMG", normMode, normRange.min,
                                  normRange.max, type, update);
    normRangeTbb.setSubscription(destId, sub("dest"));
    success = normRangeTbb.start();



    return success;
}
