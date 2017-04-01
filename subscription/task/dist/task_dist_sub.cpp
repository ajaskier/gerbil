#include "task_dist_sub.h"
#include "subscription.h"
#include "lock.h"

#include "data.h"
#include "QDebug"

#include <algorithm>

#include "distviewcompute_utils.h"
#include "labeling.h"
#include "qtopencv.h"

#include "model/labels_model.h"
#include "model/img_model.h"
#include "data_register.h"

#define REUSE_THRESHOLD    0.1

TaskDistSub::TaskDistSub(QString destId, SourceDeclaration sourceId, SourceDeclaration sourceDistId,
                         std::vector<multi_img::Value> &illuminant, bool apply,
                         bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskSub", destId, { { "source", sourceId },
                                               { "sourceDist",
                                                 sourceDistId }, { "labels", { "labels" } },
                                               { "ROI", { "ROI" } } }, illuminant), apply(apply),
    partialLabelsUpdate(partialLabelsUpdate), args(args)
{}

TaskDistSub::TaskDistSub(QString destId, SourceDeclaration sourceId,
                         std::vector<multi_img::Value> &illuminant,
                         bool apply, bool partialLabelsUpdate,
                         ViewportCtx* args)
    : TaskDistviewBinsTbb("taskSub", destId, { { "source", sourceId },
                                               { "labels",
                                                 { "labels" } }, { "ROI", { "ROI" } } }, illuminant),
    apply(apply), partialLabelsUpdate(partialLabelsUpdate), args(args)
{}

TaskDistSub::~TaskDistSub()
{}

std::vector<cv::Rect> TaskDistSub::getDiff(cv::Mat1b& mask, const multi_img &img)
{
	Subscription::Lock<cv::Rect, ROIMeta> roi_lock(*sub("ROI"));
	ROIMeta roimeta = *(roi_lock.meta());

	std::vector<cv::Rect> diff;
	if (partialLabelsUpdate) {
		if (!mask.empty()) {
			diff.push_back(cv::Rect(0, 0, mask.cols, mask.rows));
		}
//		else {
//			diff.push_back(cv::Rect(0, 0, img.width, img.height));
//		}
	} else if (roimeta.profitable) {
		diff = roimeta.sub;
	}

	return diff;
}

std::vector<BinSet> TaskDistSub::coreExecution(ViewportCtx* args, cv::Mat1s& labels,
                                               const QVector<QColor>& colors, cv::Mat1b& mask,
                                               const multi_img &img, std::vector<BinSet>* reuseDist)
{
	auto diff = getDiff(mask, img);

	bool reuse = !diff.empty();
	bool keepOldContext = false;
	//bool keepOldContext = args->valid;

	if (reuse) {
		keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
		                  (fabs(args->minval - img.minval))) &&
		                 ((fabs(args->maxval) * REUSE_THRESHOLD) >=
		                  (fabs(args->maxval - img.maxval)));

		if (!keepOldContext) {
			reuse = false;
			//diff.clear();
		}
	}

	if (diff.empty()) {
		return std::vector<BinSet>();
	}

	std::vector<BinSet> result;
	if (reuse && reuseDist) {
		result = *reuseDist;
	}
//	else {
//		result = std::vector<BinSet>();
//		diff.push_back(cv::Rect(0, 0, img.width, img.height));
//	}

	createBinSets(img, colors, result);

	if (!keepOldContext)
		updateContext(img, args);

	expression(true, diff, img, result, labels, mask, args);

	return result;
}

bool TaskDistSub::run()
{
	std::vector<BinSet> *reuseDist = nullptr;
	if (subExists("sourceDist")) {
		Subscription::Lock<std::vector<BinSet>, ViewportCtx> sourceDist_lock(*sub("sourceDist"));
		if (sourceDist_lock())
			reuseDist = sourceDist_lock();

		if (!args) {
			args = sourceDist_lock.meta();
		}
	}

	Subscription::Lock<multi_img> source_lock(*sub("source"));
	const multi_img* source = source_lock();

	Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));

	Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
	const Labels& l = *labels_lock();

	cv::Mat1b mask;
	cv::Mat1s labels;
	if (partialLabelsUpdate) {
		const LabelsMeta lMeta = *labels_lock.meta();
		mask = lMeta.mask;
		//labels = lMeta.oldLabels.clone();
		labels = mask.empty() ? l.scopedlabels.clone() : lMeta.oldLabels.clone();
	} else {
		mask   = cv::Mat1b();
		labels = l.scopedlabels;
	}

	std::vector<BinSet> result = coreExecution(args, labels, l.colors,
	                                           mask, *source, reuseDist);


	if (isCancelled()) {
		return false;
	}

	//if (!result.empty()) {
	dest_lock.swapMeta(*args);
	dest_lock.swap(result);
	//}

	return true;
}
