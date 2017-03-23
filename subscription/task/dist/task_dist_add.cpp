#include "task_dist_add.h"
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

TaskDistAdd::TaskDistAdd(QString destId, SourceDeclaration sourceImgId,
                         std::vector<multi_img::Value> &illuminant, bool apply,
                         bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskAdd", destId, { { "source", sourceImgId }, { "labels", { "labels" } },
                                               { "ROI",
                                                 { "ROI" } } },
                          illuminant), apply(apply), partialLabelsUpdate(partialLabelsUpdate),
    args(args)
{}

TaskDistAdd::TaskDistAdd(QString destId, SourceDeclaration sourceImgId,
                         SourceDeclaration sourceTempId,
                         std::vector<multi_img::Value> &illuminant, bool apply,
                         bool partialLabelsUpdate,
                         ViewportCtx *args)
    : TaskDistviewBinsTbb("taskAdd", destId, { { "source", sourceImgId }, { "temp", sourceTempId },
                                               { "labels",
                                                 { "labels" } }, { "ROI", { "ROI" } } }, illuminant),
    apply(apply), partialLabelsUpdate(partialLabelsUpdate),
    args(args)
{}

TaskDistAdd::~TaskDistAdd()
{}

std::vector<cv::Rect> TaskDistAdd::getDiff(const cv::Mat1b &mask, const multi_img& img)
{
	Subscription::Lock<cv::Rect, ROIMeta> roi_lock(*sub("ROI"));
	ROIMeta roimeta = *(roi_lock.meta());

	std::vector<cv::Rect> diff;
	if (partialLabelsUpdate) {
		if (mask.cols == 0 && mask.rows == 0) {
			diff.push_back(cv::Rect(0, 0, img.width, img.height));
		} else {
			diff.push_back(cv::Rect(0, 0, mask.cols, mask.rows));
		}
	} else if (!roimeta.profitable) {
		diff.push_back(cv::Rect(0, 0, img.width, img.height));
	} else {
		diff = roimeta.add;
	}

	return diff;
}

std::vector<BinSet> TaskDistAdd::coreExecution(ViewportCtx *args, cv::Mat1s& labels,
                                               QVector<QColor>& colors, cv::Mat1b& mask,
                                               multi_img& img, std::vector<BinSet>* reuseDist)
{
	auto diff = getDiff(mask, img);

	bool reuse = !diff.empty();
	bool keepOldContext;

	if (reuse) {
		keepOldContext = ((fabs(args->minval) * REUSE_THRESHOLD) >=
		                  (fabs(args->minval - img.minval))) &&
		                 ((fabs(args->maxval) * REUSE_THRESHOLD) >=
		                  (fabs(args->maxval - img.maxval)));

		if (!keepOldContext) {
			reuse = false;
		}
	}

	std::vector<BinSet> result;
	if (reuse && reuseDist) {
		result = *reuseDist;
	} else if (!partialLabelsUpdate) {
		diff.clear();
		diff.push_back(cv::Rect(0, 0, img.width, img.height));
	}

	createBinSets(img, colors, result);
	updateContext(img, args);

	expression(false, diff, img, result, labels, mask, args);
	return result;
}

bool TaskDistAdd::run()
{
	Subscription::Lock<std::vector<BinSet>, ViewportCtx> dest_lock(*sub("dest"));
	if (!args)
		args = dest_lock.meta();

	std::vector<BinSet> *reuseDist = nullptr;
	if (subExists("temp")) {
		Subscription::Lock<std::vector<BinSet>, ViewportCtx> temp_lock(*sub("temp"));
		if (temp_lock()) {
			reuseDist = temp_lock();
		}
	}

	Subscription::Lock<Labels, LabelsMeta> labels_lock(*sub("labels"));
	Labels l = *labels_lock();

	Subscription::Lock<multi_img> source_lock(*sub("source"));
	multi_img* source = source_lock();


	cv::Mat1b mask;
	if (partialLabelsUpdate) {
		LabelsMeta lMeta = *labels_lock.meta();
		mask = lMeta.mask;
	} else {
		mask = cv::Mat1b();
	}

	std::vector<BinSet> result = coreExecution(args, l.scopedlabels, l.colors,
	                                           mask, *source, reuseDist);

	if (isCancelled()) {
		return false;
	}

	dest_lock.swap(result);
	dest_lock.swapMeta(*args);
	dest_lock.setVersion(source_lock.version());

	return true;
}
