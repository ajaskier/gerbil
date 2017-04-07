#include "task_graph_seg.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"

#include "sm_config.h"
#include "sm_factory.h"
#include "similarity_measure.h"

#include "task/labels/task_alter_label.h"

TaskGraphSeg::TaskGraphSeg(QString sourceId, const seg_graphs::GraphSegConfig config,
                           const cv::Mat1s & seedMap,
                           bool resetLabel, int label)
    : Task("labels", { { "source", { sourceId } } }),
    config(config), seedMap(seedMap), resetLabel(resetLabel), label(label)
{}

TaskGraphSeg::~TaskGraphSeg() {}

bool TaskGraphSeg::run()
{
	if (resetLabel) {
		cv::Mat1b emptyMat;

		TaskAlterLabel task(label, emptyMat, false);
		task.importSubscription(sub("dest"));
		auto success = task.start();

		if (!success)
			return success;
	}

	cv::Mat1s result;
	{
		Subscription::Lock<multi_img> lock(*sub("source"));
		const multi_img* const        source = lock();

		const multi_img* const input = new multi_img(*source);

		seg_graphs::GraphSeg seg(config);
		result = seg.execute(*input, seedMap);
	}

	{
		TaskAlterLabel task(label, result, false);
		task.importSubscription(sub("dest"));
		auto success = task.start();

		if (!success)
			return success;
	}

	return true;
}

