#include "task_normrange_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "task_band2qimage_tbb.h"

#include <QImage>

TaskNormRangeTbb::TaskNormRangeTbb(QString destId, multi_img::NormMode mode,
                                   multi_img::Value minval,
                                   multi_img::Value maxval,
                                   representation::t type, bool update)
	: TaskDataRangeTbb(destId), mode(mode), minval(minval),
	maxval(maxval), type(type), update(update)
{}

TaskNormRangeTbb::~TaskNormRangeTbb()
{}

bool TaskNormRangeTbb::run()
{
	multi_img_base::Range range;

	switch (mode) {
	case multi_img::NORM_OBSERVED:
		if (!TaskDataRangeTbb::run())
			return false;
		break;
	case multi_img::NORM_THEORETICAL:

		switch (type) {
		case representation::IMG:
			range.min = (multi_img::Value)MULTI_IMG_MIN_DEFAULT;
			range.max = (multi_img::Value)MULTI_IMG_MAX_DEFAULT;
			break;
		case representation::GRAD:
			range.min = (multi_img::Value)-log(MULTI_IMG_MAX_DEFAULT);
			range.max = (multi_img::Value)log(MULTI_IMG_MAX_DEFAULT);
			break;
		case representation::NORM:
			range.min = 0;
			range.max = 1;
			break;
		case representation::IMGPCA:
			range.min = 4 * (multi_img::Value)MULTI_IMG_MIN_DEFAULT;
			range.max = 4 * (multi_img::Value)MULTI_IMG_MAX_DEFAULT;
			break;
		case representation::GRADPCA:
			range.min = 4 * (multi_img::Value)-log(MULTI_IMG_MAX_DEFAULT);
			range.max = 4 * (multi_img::Value)log(MULTI_IMG_MAX_DEFAULT);
		}
		break;
	default:
		range.min = minval;
		range.max = maxval;
		break;
	}

	if (isCancelled())
		return false;

	Subscription::Lock<multi_img, multi_img_base::Range> dest_lock(*sub("dest"));
	dest_lock.swapMeta(range);

	if (update) {
		dest_lock()->minval = range.min;
		dest_lock()->maxval = range.max;
	}
	return true;
}
