#include "task_labels_consolidate.h"

#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#include "task_set_labels.h"

#define REUSE_THRESHOLD 0.1

TaskLabelsConsolidate::TaskLabelsConsolidate(const cv::Size originalImageSize)
    : Task("consolidateLabels", "labels", { {"ROI", {"ROI"}} }),
    originalImageSize(originalImageSize)
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

    TaskSetLabels setLabelsTask(newfull, originalImageSize, true);
	setLabelsTask.importSubscription(sub("dest"));
	setLabelsTask.importSubscription(sub("ROI"));

	return setLabelsTask.start();
}

