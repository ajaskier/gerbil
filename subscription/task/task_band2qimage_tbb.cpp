#include "task_band2qimage_tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_offloaded.h"
#include "imginput.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <background_task/tasks/tbb/band2qimagetbb.h>

TaskBand2QImageTbb::TaskBand2QImageTbb(QString sourceId, QString destId,
                                       size_t dim)
    : Task(destId, {{sourceId, "source"}}), dim(dim)
{
}

TaskBand2QImageTbb::~TaskBand2QImageTbb()
{
}

bool TaskBand2QImageTbb::run()
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    multi_img* multi = source_lock();

    if (dim >= multi->size()) {
        return false;
    }

    multi_img::Band &source = multi->bands[dim];
    QImage* target = new QImage(source.cols, source.rows, QImage::Format_ARGB32);
    Band2QImage converter(source, *target, multi->minval, multi->maxval);
    tbb::parallel_for(tbb::blocked_range2d<int>(0, source.rows, 0, source.cols),
                      converter, tbb::auto_partitioner(), stopper);

    if (stopper.is_group_execution_cancelled()) {
        delete target;
        return false;
    } else {
        std::pair<QImage, QString> data(*target, QString());

        Subscription::Lock<std::pair<QImage, QString>> dest_lock(*sub("dest"));
        dest_lock.swap(data);
        return true;
    }
}
