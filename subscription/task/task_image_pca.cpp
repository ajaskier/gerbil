#include "task_image_pca.h"
#include "subscription.h"

#include "task_pca_tbb.h"
#include "task_normrange_tbb.h"

#include <QDebug>

TaskImagePCA::TaskImagePCA(QString sourceId, QString destId,
                           multi_img::NormMode normMode,
                           multi_img_base::Range normRange,
                           representation::t type, bool update,
                           unsigned int components, bool includecache)
    : Task(destId, {{sourceId, "source"}}), normMode(normMode),
      normRange(normRange), type(type), update(update),
      components(components), includecache(includecache)
{
}

TaskImagePCA::~TaskImagePCA()
{}

bool TaskImagePCA::run()
{
    auto sourceId = sub("source")->getDependency().dataId;
    auto destId = sub("dest")->getDependency().dataId;

    TaskPcaTbb taskPca(sourceId, destId, components, includecache);
    taskPca.setSubscription(sourceId, sub("source"));
    taskPca.setSubscription(destId, sub("dest"));
    auto success = taskPca.start();
    if (!success) return success;

    TaskNormRangeTbb normRangeTbb(destId, normMode, normRange.min,
                                  normRange.max, type, update);
    normRangeTbb.setSubscription(destId, sub("dest"));
    return normRangeTbb.start();
}
