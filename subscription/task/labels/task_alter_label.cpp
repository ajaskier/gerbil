#include "task_alter_label.h"

#include "subscription.h"
#include "lock.h"
#include "data_register.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD    0.1

TaskAlterLabel::TaskAlterLabel(short index, cv::Mat1b mask,
                               bool negative)
    : Task("alterLabel", "labels", {}),
    index(index), mask(mask), negative(negative)
{}

bool TaskAlterLabel::run()
{
	if (!DataRegister::isInitialized("labels")) {
		return false;
	}
	Subscription::Lock<Labels, LabelsMeta> lock(*sub("dest"));
	Labels* dst = lock();

	if (mask.empty()) {
		mask = (dst->scopedlabels == index);
		dst->scopedlabels.setTo(0, mask);
	} else if (negative) {
		mask = mask.mul(dst->scopedlabels == index);
		dst->scopedlabels.setTo(0, mask);
	} else {
		dst->scopedlabels.setTo(index, mask);
	}

	lock.swap(*dst);
	return true;
}
