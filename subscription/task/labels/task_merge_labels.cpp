#include "task_merge_labels.h"

#include "subscription.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD 0.1

TaskMergeLabels::TaskMergeLabels(const QVector<int> mlabels)
    : Task("mergeLabels", "labels", {}), mlabels(mlabels)
{
}

bool TaskMergeLabels::run()
{
    Subscription::Lock<Labels> dest_lock(*sub("dest"));
    Labels l = *dest_lock();

    // sort the labels to merge by id
    qSort(mlabels);

    // target: the label to merge into
    short target = mlabels[0];

    // mask: all pixels which are to be merged into the target label
    cv::Mat1b mask = cv::Mat1b::zeros(l.fullLabels.rows, l.fullLabels.cols);

    // build mask and new color array
    for(int i=1; i<mlabels.size(); i++) {
        short label = mlabels.at(i);
        cv::Mat1b dmask = (l.fullLabels == label);
        mask = mask | dmask;
    }

    l.fullLabels.setTo(target, mask);

    dest_lock.swap(l);

    return true;

}
