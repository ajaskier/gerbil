#include "task_image_bgr_tbb.h"
#include "subscription.h"
#include "lock.h"

#include <opencv2/core.hpp>
#include "multi_img/multi_img_tbb.h"
#include "multi_img/cieobserver.h"

TaskImageBgrTbb::TaskImageBgrTbb(QString destId, QString sourceId)
    : Task(destId, {{"source", {sourceId}}})
{
}

bool TaskImageBgrTbb::run()
{

    Subscription::Lock<multi_img_base> lock(*sub("source"));
    multi_img_base& source = *lock();

    cv::Mat_<cv::Vec3f> xyz(source.height, source.width, 0.);
    float greensum = 0.f;
    for (unsigned int i = 0; i < source.size(); ++i) {
        int idx = ((int)(source.meta[i].center + 0.5f) - 360) / 5;
        if (idx < 0 || idx > 94)
            continue;

        multi_img::Band band;
        source.getBand(i, band);
        Xyz computeXyz(source, xyz, band, idx);
        tbb::parallel_for(tbb::blocked_range2d<int>(0, xyz.rows, 0, xyz.cols),
            computeXyz, tbb::auto_partitioner(), stopper);

        greensum += CIEObserver::y[idx];

        if (isCancelled())
            break;
    }

    if (greensum == 0.f)
        greensum = 1.f;

    cv::Mat_<cv::Vec3f> newBgr(source.height, source.width);
    Bgr computeBgr(source, xyz, newBgr, greensum);
    tbb::parallel_for(tbb::blocked_range2d<int>(0, newBgr.rows, 0, newBgr.cols),
        computeBgr, tbb::auto_partitioner(), stopper);

    if (stopper.is_group_execution_cancelled()) {
        return false;
    }

    Subscription::Lock<cv::Mat3f> dest_lock(*sub("dest"));
    dest_lock.swap(newBgr);
    dest_lock.setVersion(lock.version());

    return true;
}
