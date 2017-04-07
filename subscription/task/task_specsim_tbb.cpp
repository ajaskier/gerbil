#include "task_specsim_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"

#include <background_task/tasks/tbb/band2qimagetbb.h>


#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <QDebug>

#include "sm_config.h"
#include "sm_factory.h"
#include "similarity_measure.h"

using SimMeasure = similarity_measures::SimilarityMeasure<multi_img::Value>;

TaskSpecSimTbb::TaskSpecSimTbb(int x, int y, similarity_measures::SMConfig config)
    : TbbTask("similarity", { { "source", { "image.IMG" } } }), x(x), y(y), config(config)
{}

TaskSpecSimTbb::~TaskSpecSimTbb()
{}

bool TaskSpecSimTbb::run()
{
	Subscription::Lock<multi_img> lock(*sub("source"));
	const multi_img* const        source = lock();

	SimMeasure *distfun = similarity_measures::SMFactory<multi_img::Value>::spawn(config);
	//cv::Point point(x, y);

	multi_img::Band         result(source->height, source->width);
	const multi_img::Pixel& reference = (*source)(x, y);

	tbb::parallel_for(tbb::blocked_range2d<size_t>(
	                      0, source->height, 0, source->width),
	                  [&](tbb::blocked_range2d<size_t> r) {
		                  for (size_t y = r.rows().begin(); y != r.rows().end(); ++y) {
							  for (size_t x = r.cols().begin(); x != r.cols().end(); ++x) {
								  // negate so small values get high response
								  result(y,
								         x) = -1.f *
								              (float)distfun->getSimilarity((*source)(y,
								                                                      x),
								                                            reference);
							  }
						  }
	                  }
	                  );

	double min;
	double max;
	cv::minMaxLoc(result, &min, &max);
	multi_img::Value minval = (multi_img::Value)min;
	multi_img::Value maxval = 0;     // 0 -> equal // (multi_img::Value)max;

	QImage      target(result.cols, result.rows, QImage::Format_ARGB32);
	Band2QImage computeConversion(result, target, minval, maxval);
	tbb::parallel_for(tbb::blocked_range2d<int>(0, result.rows, 0, result.cols),
	                  computeConversion, tbb::auto_partitioner(), stopper);

	if (stopper.is_group_execution_cancelled()) {
		return false;
	} else {
		Subscription::Lock<QImage> dest_lock(*sub("dest"));
		dest_lock.swap(target);
		return true;
	}
}

