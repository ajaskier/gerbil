#include "task_roi.h"
#include "subscription.h"
#include "lock.h"
#include "QDebug"

#include <rectangles.h>

#include "model/img_model.h"

#define REUSE_THRESHOLD    0.1

TaskRoi::TaskRoi(cv::Rect oldRoi, cv::Rect newRoi)
	: Task("ROI", {}), oldRoi(oldRoi), newRoi(newRoi)
{}



TaskRoi::~TaskRoi()
{}

bool TaskRoi::run()
{
	qDebug() << "about to set roi width:" << newRoi.width << "height:" << newRoi.height;
	ROIMeta meta;

	meta.profitable = rectTransform(oldRoi, newRoi, meta.sub, meta.add);

	Subscription::Lock<cv::Rect, ROIMeta> lock(*sub("dest"));
	lock.swap(newRoi);
	lock.swapMeta(meta);
	lock.setVersion(lock.version() + 1);

	return true;
}
