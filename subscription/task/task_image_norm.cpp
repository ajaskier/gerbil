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
    TaskNormL2Tbb taskNorm;
	taskNorm.importSubscription(sub("source"));
	taskNorm.importSubscription(sub("dest"));
    auto success = taskNorm.start();
    if (!success) return success;

    TaskNormRangeTbb normRangeTbb("image.NORM", normMode, normRange.min,
                                  normRange.max, type, update);
	normRangeTbb.importSubscription(sub("dest"));
    return normRangeTbb.start();
}
