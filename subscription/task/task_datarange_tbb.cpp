#include "task_datarange_tbb.h"
#include "subscription.h"
#include "lock.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_reduce.h>

#include "multi_img/multi_img_tbb.h"


TaskDataRangeTbb::TaskDataRangeTbb(QString destId)
    : Task(destId, {})
{
}

TaskDataRangeTbb::~TaskDataRangeTbb()
{
}

bool TaskDataRangeTbb::run()
{
    Subscription::Lock<multi_img, multi_img_base::Range> dest_lock(*sub("dest"));
    multi_img* dest = dest_lock();

    DetermineRange determineRange(*dest);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, dest->size()),
                         determineRange, tbb::auto_partitioner(), stopper);


    if (!isCancelled()) {
        multi_img_base::Range range;
        range.min = determineRange.GetMin();
        range.max = determineRange.GetMax();
        dest_lock.swapMeta(range);

        return true;
    } else {
        return false;
    }
}
