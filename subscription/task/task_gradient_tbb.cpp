#include "task_gradient_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"
#include "rectangles.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

TaskGradientTbb::TaskGradientTbb(bool includecache)
	: Task("image.GRAD", { { "source", { "image.IMG" } } }), includecache(includecache)
{}

TaskGradientTbb::~TaskGradientTbb()
{}

bool TaskGradientTbb::run()
{
	Subscription::Lock<multi_img> source_lock(*sub("source"));
	Subscription::Lock<multi_img> dest_lock(*sub("dest"));

	multi_img* source = source_lock();
	multi_img* dest   = dest_lock();

	cv::Rect copyGlob;
	cv::Rect copySrc(0, 0, 0, 0);
	cv::Rect copyCur(0, 0, 0, 0);

	if (dest) {
		copyGlob = source->roi & dest->roi;


		if (copyGlob.width > 0 && copyGlob.height > 0) {
			copySrc.x      = copyGlob.x - source->roi.x;
			copySrc.y      = copyGlob.y - source->roi.y;
			copySrc.width  = copyGlob.width;
			copySrc.height = copyGlob.height;

			copyCur.x = copyGlob.x - dest->roi.x;
			copyCur.y = copyGlob.y - dest->roi.y;

			copyCur.width  = copyGlob.width;
			copyCur.height = copyGlob.height;
		}
	}

	std::vector<cv::Rect> calc;
	rectComplement(source->width, source->height, copySrc, calc);

	multi_img *target = new multi_img(
		source->height, source->width, source->size() - 1);

	if (dest) {
		if (copyGlob.width > 0 && copyGlob.height > 0) {
			for (size_t i = 0; i < target->size(); ++i) {
				multi_img::Band curBand = dest->bands[i](copyCur);
				multi_img::Band tgtBand = target->bands[i](copySrc);
				curBand.copyTo(tgtBand);

				if (isCancelled())
					break;
			}
		}
	}

	std::vector<cv::Rect>::iterator it;
	multi_img temp(source->height, source->width, source->size());
	for (it = calc.begin(); it != calc.end(); ++it) {
		if (it->width > 0 && it->height > 0) {
			multi_img srcScope(*source, *it);
			multi_img tmpScope(temp, *it);
			Log       computeLog(srcScope, tmpScope);
			tbb::parallel_for(tbb::blocked_range<size_t>(0, temp.size()),
			                  computeLog, tbb::auto_partitioner(), stopper);
		}

		if (isCancelled())
			break;
	}
	temp.minval = 0.;
	temp.maxval = log(source->maxval);
	temp.roi    = source->roi;

	for (it = calc.begin(); it != calc.end(); ++it) {
		if (it->width > 0 && it->height > 0) {
			multi_img tmpScope(temp, *it);
			multi_img tgtScope(*target, *it);
			Grad      computeGrad(tmpScope, tgtScope);
			tbb::parallel_for(tbb::blocked_range<size_t>(0, target->size()),
			                  computeGrad, tbb::auto_partitioner(), stopper);
		}

		if (isCancelled())
			break;
	}
	target->minval = -temp.maxval;
	target->maxval = temp.maxval;
	target->roi    = temp.roi;

	// init multi_img::meta
	for (unsigned int i = 0; i < source->size() - 1; ++i) {
		if (!source->meta[i].empty && !source->meta[i + 1].empty) {
			target->meta[i] = multi_img::BandDesc(
				source->meta[i].center,
				source->meta[i + 1].center);
		}
	}

	if (includecache) {
		RebuildPixels rebuildPixels(*target);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, target->size()),
		                  rebuildPixels, tbb::auto_partitioner(), stopper);
		target->dirty.setTo(0);
		target->anydirt = false;
	} else {
		target->resetPixels();
	}

	if (isCancelled()) {
		delete target;
		return false;
	} else {
		dest_lock.swap(*target);
		return true;
	}
}
