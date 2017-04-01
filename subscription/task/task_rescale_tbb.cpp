#include "task_rescale_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <QDebug>


TaskRescaleTbb::TaskRescaleTbb(size_t bands, size_t roiBands,
                               bool includecache)
	: Task("image.IMG", { { "source", { "image" } } }),
	bands(bands), roiBands(roiBands),
	includecache(includecache)
{}

TaskRescaleTbb::~TaskRescaleTbb()
{}

bool TaskRescaleTbb::run()
{
	qDebug() << "rescale started";

	/* sanitize number of bands */
	size_t maxBands;
	{
		Subscription::Lock<multi_img_base> source(*sub("source"));
		multi_img_base * img = source();
		maxBands = img->size();
	}
	if ((bands < 1 && roiBands < 1)
	    || bands > maxBands) {
		newsize = maxBands;
	} else if (bands < 1) {
		newsize = roiBands;
	} else if (bands <= 2) {
		newsize = 3;
	}

	Subscription::Lock<multi_img> dest(*sub("dest"));

	multi_img* temp = new multi_img(*dest(), cv::Rect(0, 0, dest()->width,
	                                                  dest()->height));
	temp->roi = dest()->roi;
	RebuildPixels rebuildPixels(*temp);
	tbb::parallel_for(tbb::blocked_range<size_t>(0, temp->size()),
	                  rebuildPixels, tbb::auto_partitioner(), stopper);

	temp->dirty.setTo(0);
	temp->anydirt = false;

	multi_img *target = nullptr;
	if (newsize != temp->size()) {
		target = new multi_img(dest()->height, dest()->width,
		                       newsize);
		target->minval = temp->minval;
		target->maxval = temp->maxval;
		target->roi    = temp->roi;
		Resize computeResize(*temp, *target, newsize);
		tbb::parallel_for(tbb::blocked_range2d<int>(0, temp->height,
		                                            0, temp->width),
		                  computeResize, tbb::auto_partitioner(), stopper);

		ApplyCache applyCache(*target);
		tbb::parallel_for(tbb::blocked_range<size_t>(0, target->size()),
		                  applyCache, tbb::auto_partitioner(), stopper);
		target->dirty.setTo(0);
		target->anydirt = false;

		if (!isCancelled()) {
			cv::Mat_<float> tmpmeta1(cv::Size(temp->meta.size(),
			                                  1)), tmpmeta2;
			std::vector<multi_img::BandDesc>::const_iterator it;
			unsigned int i;
			for (it = temp->meta.begin(), i = 0; it != temp->meta.end(); it++, i++) {
				tmpmeta1(0, i) = it->center;
			}
			cv::resize(tmpmeta1, tmpmeta2, cv::Size(newsize, 1));
			for (size_t b = 0; b < newsize; b++) {
				target->meta[b] = multi_img::BandDesc(tmpmeta2(0, b));
			}
		}

		delete temp;
		temp = nullptr;
	} else {
		target = temp;
	}

	if (!includecache)
		target->resetPixels();

	if (isCancelled()) {
		delete target;
		return false;
	} else {
		dest.swap(*target);
		return true;
	}

	qDebug() << "rescale finished";
}

