#include "task_image_norm.h"
#include "subscription.h"

#include "task_norml2tbb.h"
#include "task_normrange_tbb.h"

#include <QDebug>

TaskImageNORM::TaskImageNORM(multi_img::NormMode normMode,
                             multi_img_base::Range normRange,
                             representation::t type, bool update)
    : Task("image.NORM", {{"source", {"image.IMG"}}}), normMode(normMode),
      normRange(normRange), type(type), update(update)
{
}

TaskImageNORM::~TaskImageNORM()
{}

bool TaskImageNORM::run()
{
    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;

    TaskNormL2Tbb taskNorm;
    taskNorm.setSubscription(sourceId, sub("source"));
    taskNorm.setSubscription(destId, sub("dest"));
    auto success = taskNorm.start();
    if (!success) return success;

    TaskNormRangeTbb normRangeTbb("image.NORM", normMode, normRange.min,
                                  normRange.max, type, update);
    normRangeTbb.setSubscription(destId, sub("dest"));
    return normRangeTbb.start();
}
