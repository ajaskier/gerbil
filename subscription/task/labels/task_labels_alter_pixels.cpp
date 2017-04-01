#include "task_labels_alter_pixels.h"

#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD    0.1

TaskLabelsAlterPixels::TaskLabelsAlterPixels(const cv::Mat1s &newLabels, const cv::Mat1b &mask)
    : Task("labelsAlterPixels", "labels", { { "ROI", { "ROI" } } }),
    newLabels(newLabels), mask(mask)
{}

bool TaskLabelsAlterPixels::run()
{
	Subscription::Lock<Labels, LabelsMeta> dest_lock(*sub("dest"));
	Labels l = *dest_lock();

	LabelsMeta lMeta;
	lMeta.oldLabels = l.fullLabels.clone();
	lMeta.mask      = mask.clone();

	newLabels.copyTo(l.scopedlabels, mask);

	dest_lock.swap(l);
	dest_lock.swapMeta(lMeta);

	Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
	dest_lock.setVersion(roi_lock.version());

	return true;
}
