#include "task_image_img.h"
#include "subscription.h"
#include "lock.h"

#include "task_scope_image.h"
#include "task_rescale_tbb.h"
#include "task_normrange_tbb.h"
#include <QDebug>

TaskImageIMG::TaskImageIMG(cv::Rect roi, size_t bands, size_t roiBands,
                           multi_img::NormMode normMode,
                           multi_img_base::Range normRange,
                           representation::t type, bool update, bool includecache)
    : Task("image.IMG", {{"image", "source"}}), roi(roi), bands(bands),
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

    TaskScopeImage scopeImage(roi);
    scopeImage.setSubscription(sourceId, sub("source"));
    scopeImage.setSubscription(destId, sub("dest"));
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

    Subscription::Lock<multi_img> source_lock(*sub("source"));
    Subscription::Lock<multi_img> dest_lock(*sub("dest"));
   // dest_lock.setVersion(dest_lock.version()+1);

    dest_lock.setVersion(source_lock.version());

    return success;
}
