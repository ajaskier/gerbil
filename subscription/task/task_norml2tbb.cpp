#include "task_norml2tbb.h"
#include "subscription.h"
#include "lock.h"

#include "multi_img.h"
#include "multi_img/multi_img_tbb.h"
#include "rectangles.h"

#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>

TaskNormL2Tbb::TaskNormL2Tbb(QObject *parent)
    : Task("image.NORM", parent)
{
    dependencies = {Dependency("image.NORM", SubscriptionType::WRITE),
                    Dependency("image.IMG", SubscriptionType::READ)};
}

TaskNormL2Tbb::~TaskNormL2Tbb()
{

}

void TaskNormL2Tbb::run()
{
    Subscription::Lock<multi_img> source_lock(*sourceSub);
    Subscription::Lock<multi_img> current_lock(*currentSub);

    multi_img* source = source_lock();
    multi_img* current = current_lock();

    cv::Rect copyGlob;
    cv::Rect copySrc(0, 0, 0, 0);
    cv::Rect copyCur(0, 0, 0, 0);

    if (current) {
        copyGlob = source->roi & current->roi;


        if (copyGlob.width > 0 && copyGlob.height > 0) {
            copySrc.x = copyGlob.x - source->roi.x;
            copySrc.y = copyGlob.y - source->roi.y;
            copySrc.width = copyGlob.width;
            copySrc.height = copyGlob.height;

            copyCur.x = copyGlob.x - current->roi.x;
            copyCur.y = copyGlob.y - current->roi.y;

            copyCur.width = copyGlob.width;
            copyCur.height = copyGlob.height;
        }
    }

    std::vector<cv::Rect> calc;
    rectComplement(source->width, source->height, copySrc, calc);

    multi_img* target = new multi_img(
                source->height, source->width, source->size());

    if (current) {
        if (copyGlob.width > 0 && copyGlob.height > 0) {
            for(size_t i = 0; i < target->size(); i++) {
                multi_img::Band curBand = current->bands[i](copyCur);
                multi_img::Band tgtBand = target->bands[i](copySrc);
                curBand.copyTo(tgtBand);

                if (stopper.is_group_execution_cancelled())
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

        if (stopper.is_group_execution_cancelled())
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

    if (stopper.is_group_execution_cancelled()) {
        delete target;
        return;
    } else {
        current_lock.swap(*target);
        return;
    }

}

void TaskNormL2Tbb::setSubscription(QString id, std::shared_ptr<Subscription> sub)
{
    if (id == "image.NORM") currentSub = sub;
    else if (id == "image.IMG") sourceSub = sub;
}
