#include "task_norml2tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"
#include "rectangles.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

TaskNormL2Tbb::TaskNormL2Tbb()
    : Task("image.NORM", {{"image.IMG", "source"}})
{
}

TaskNormL2Tbb::~TaskNormL2Tbb()
{

}

bool TaskNormL2Tbb::run()
{
    Subscription::Lock<multi_img> source_lock(*sub("source"));
    Subscription::Lock<multi_img> dest_lock(*sub("dest"));

    multi_img* source = source_lock();
    multi_img* dest = dest_lock();

    cv::Rect copyGlob;
    cv::Rect copySrc(0, 0, 0, 0);
    cv::Rect copyCur(0, 0, 0, 0);

    if (dest) {
        copyGlob = source->roi & dest->roi;


        if (copyGlob.width > 0 && copyGlob.height > 0) {
            copySrc.x = copyGlob.x - source->roi.x;
            copySrc.y = copyGlob.y - source->roi.y;
            copySrc.width = copyGlob.width;
            copySrc.height = copyGlob.height;

            copyCur.x = copyGlob.x - dest->roi.x;
            copyCur.y = copyGlob.y - dest->roi.y;

            copyCur.width = copyGlob.width;
            copyCur.height = copyGlob.height;
        }
    }

    std::vector<cv::Rect> calc;
    rectComplement(source->width, source->height, copySrc, calc);

    multi_img* target = new multi_img(
                source->height, source->width, source->size());

    if (dest) {
        if (copyGlob.width > 0 && copyGlob.height > 0) {
            for(size_t i = 0; i < target->size(); i++) {
                multi_img::Band curBand = dest->bands[i](copyCur);
                multi_img::Band tgtBand = target->bands[i](copySrc);
                curBand.copyTo(tgtBand);

                if (isCancelled())
                    break;
            }

            /* reconstruct pixel cache for the region */
            RebuildPixels rebuildPixels(*target);
            tbb::parallel_for(tbb::blocked_range2d<int>(copySrc.y, copySrc.br().y,
                                                        copySrc.x, copySrc.br().x),
                rebuildPixels, tbb::auto_partitioner(), stopper);
        }
    }


    // second: compute missing parts
    std::vector<cv::Rect>::iterator it;
    for (it = calc.begin(); it != calc.end(); ++it) {
        if (it->width > 0 && it->height > 0) {
            NormL2 computeNormL2(*source, *target);
            tbb::parallel_for(tbb::blocked_range2d<int>(it->y, it->br().y,
                                                        it->x, it->br().x),
                computeNormL2, tbb::auto_partitioner(), stopper);

            // apply the new vectors to band data
            ApplyCache applyCache(*target);
            tbb::parallel_for(tbb::blocked_range2d<int>(it->y, it->br().y,
                                                        it->x, it->br().x),
                applyCache, tbb::auto_partitioner(), stopper);
        }

        if (isCancelled())
            break;
    }

    /* we either rebuilt the pixels, or had computation happen in the cache. */
    target->dirty.setTo(0);
    target->anydirt = false;

    target->minval = 0.f;
    target->maxval = 1.f;
    target->roi = source->roi;

    // init multi_img::meta
    target->meta = source->meta;

    if (isCancelled()) {
        delete target;
        return false;
    } else {
        dest_lock.swap(*target);
        return true;
    }

}
