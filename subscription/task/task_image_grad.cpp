#include "task_image_grad.h"
#include "subscription.h"

#include "task_gradient_tbb.h"
#include "task_normrange_tbb.h"

#include <QDebug>

TaskImageGRAD::TaskImageGRAD(multi_img::NormMode normMode,
                             multi_img_base::Range normRange,
                             representation::t type, bool update,
                             bool includecache)
	: Task("image.GRAD", { { "source", { "image.IMG" } } }), normMode(normMode),
	normRange(normRange), type(type), update(update), includecache(includecache)
{}

TaskImageGRAD::~TaskImageGRAD()
{}

bool TaskImageGRAD::run()
{
	TaskGradientTbb taskGrad(includecache);
	taskGrad.importSubscription(sub("source"));
	taskGrad.importSubscription(sub("dest"));
	auto success = taskGrad.start();
	if (!success)
		return success;

	TaskNormRangeTbb normRangeTbb("image.GRAD", normMode, normRange.min,
	                              normRange.max, type, update);
	normRangeTbb.importSubscription(sub("dest"));
	return normRangeTbb.start();
}
