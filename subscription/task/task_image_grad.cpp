#include "task_image_grad.h"
#include "subscription.h"

#include "task_gradient_tbb.h"
#include "task_normrange_tbb.h"

#include <QDebug>

TaskImageGRAD::TaskImageGRAD(multi_img::NormMode normMode,
                             multi_img_base::Range normRange,
                             representation::t type, bool update,
                             bool includecache)
    : Task("image.GRAD", {{"source", {"image.IMG"}}}), normMode(normMode),
      normRange(normRange), type(type), update(update), includecache(includecache)
{
}

TaskImageGRAD::~TaskImageGRAD()
{}

bool TaskImageGRAD::run()
{
    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;

    TaskGradientTbb taskGrad(includecache);
    taskGrad.setSubscription(sourceId, sub("source"));
    taskGrad.setSubscription(destId, sub("dest"));
    auto success = taskGrad.start();
    if (!success) return success;

    TaskNormRangeTbb normRangeTbb("image.GRAD", normMode, normRange.min,
                                  normRange.max, type, update);
    normRangeTbb.setSubscription(destId, sub("dest"));
    return normRangeTbb.start();
}
