#include "task_pca_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"
#include "rectangles.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

#include <tbb/tbb.h>

TaskPcaTbb::TaskPcaTbb(QString sourceId, QString destId, unsigned int components, bool includecache)
    : TbbTask(destId, { { "source", sourceId } }), components(components),
	includecache(includecache)
{}

TaskPcaTbb::~TaskPcaTbb()
{}

bool TaskPcaTbb::run()
{
	Subscription::Lock<multi_img> source_lock(*sub("source"));
	multi_img * source = source_lock();

	cv::Mat_<multi_img::Value> pixels(
	    source->size(), source->width * source->height);
	MultiImg2BandMat computePixels(*source, pixels);
	tbb::parallel_for(tbb::blocked_range<size_t>(0, source->size()),
	                  computePixels, tbb::auto_partitioner(), stopper);

	cv::PCA pca(pixels, cv::noArray(), CV_PCA_DATA_AS_COL, (int)components);

	multi_img *target = new multi_img(
	    source->height, source->width, pca.eigenvectors.rows);
	PcaProjection computeProjection(pixels, *target, pca);
	tbb::parallel_for(tbb::blocked_range<size_t>(0, target->pixels.size()),
	                  computeProjection, tbb::auto_partitioner(), stopper);

	ApplyCache applyCache(*target);
	tbb::parallel_for(tbb::blocked_range<size_t>(0, target->size()),
	                  applyCache, tbb::auto_partitioner(), stopper);

	DetermineRange determineRange(*target);
	tbb::parallel_reduce(tbb::blocked_range<size_t>(0, target->size()),
	                     determineRange, tbb::auto_partitioner(), stopper);

	if (!isCancelled()) {
		target->dirty.setTo(0);
		target->anydirt = false;
		target->minval  = determineRange.GetMin();
		target->maxval  = determineRange.GetMax();
		target->roi     = source->roi;
	}

	if (!includecache)
		target->resetPixels();

	if (isCancelled()) {
		delete target;
		return false;
	} else {
		Subscription::Lock<multi_img> dest_lock(*sub("dest"));
		dest_lock.swap(*target);
		return true;
	}
}
