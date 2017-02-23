#include "task_labels_consolidate.h"

#include "subscription.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#include "task_set_labels.h"

#define REUSE_THRESHOLD 0.1

TaskLabelsConsolidate::TaskLabelsConsolidate()
    : Task("consolidateLabels", "labels", { {"ROI", {"ROI"}} })
{
}

bool TaskLabelsConsolidate::run()
{
    Labeling newfull;
    {
        Subscription::Lock<Labels> dest_lock(*sub("dest"));
        Labels l = *dest_lock();
        newfull = Labeling(l.fullLabels);
    }
    newfull.consolidate();

    auto destId = sub("dest")->getDependency().dataId;
    auto roiId = sub("ROI")->getDependency().dataId;


    TaskSetLabels setLabelsTask(newfull, true);
    setLabelsTask.setSubscription(destId, sub("dest"));
    setLabelsTask.setSubscription(roiId, sub("ROI"));
    bool success = setLabelsTask.start();
    return success;

}

