#include "task_labels_icons.h"

#include "subscription.h"
#include "lock.h"

#include "data.h"
#include <QDebug>
#include <QPainter>
#include <QImage>

#include "labeling.h"
#include "qtopencv.h"

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#define REUSE_THRESHOLD 0.1

//! Computes a scaling factor, so that the scaled src image size fits entirely
//! into the destination size, even if aspect ratio of src and dst differ.
inline float scaleToFit2(const cv::Size& src,
                         const cv::Size& dst)
{
    const float rsrc = float(src.width)/src.height;
    const float rdst = float(dst.width)/dst.height;
    float scale;
    if(rsrc >= rdst) {
        scale = float(dst.width)/src.width;
    } else {
        scale = float(dst.height)/src.height;
    }
    return scale;
}


//! Clamp an integer value into the range [min, max].
int clamp2(const int& val, const int& min, const int& max)
{
    if(val < min) {
        return min;
    } else if(max < val) {
        return max;
    } else {
        return val;
    }
}

// TBB functor
class ComputeIconMasks {
public:
    ComputeIconMasks(IconTaskCtx& ctx)
        : ctx(ctx)
    {
        // clamp the icon size
        iconSizecv = cv::Size(
                    clamp2(ctx.iconSize.width(),
                          TaskLabelsIcons::IconSizeMin,
                          TaskLabelsIcons::IconSizeMax),
                    clamp2(ctx.iconSize.height(),
                          TaskLabelsIcons::IconSizeMin,
                          TaskLabelsIcons::IconSizeMax));

        // inner size = icon size without border (fixed to 1px)
        innerSize = QSize(iconSizecv.width - 2,
                          iconSizecv.height - 2);

        innerSizecv = cv::Size(innerSize.width(), innerSize.height());

        if (ctx.applyROI) {
            labels = ctx.roi_labels;
        } else {
            labels = ctx.full_labels;
        }

        scale = scaleToFit2(labels.size(), innerSizecv);

        // offset into icon rect
        dx = 0.5 * (float(iconSizecv.width) - labels.cols*scale);
        dy = 0.5 * (float(iconSizecv.height) - labels.rows*scale);

        // affine trafo matrix
        trafo = cv::Mat1f::zeros(2,3);
        trafo(0,0) = scale;
        trafo(1,1) = scale;
        trafo(0,2) = dx;
        trafo(1,2) = dy;

        // rect of the transformed mask in the icon
        drect = QRectF(dx, dy,
                       labels.cols*scale, labels.rows*scale);
        // rect of the border around the transformed mask
        brect = QRectF(drect.left(), drect.top(),
                       drect.width()-1, drect.height()-1);

    }

    void operator()(const tbb::blocked_range<short>& range) const {
        for (short labelid=range.begin(); labelid!=range.end(); ++labelid) {
            // Compute mask.
            // For big images it might make sense to parallelize this on a
            // smaller granularity (pixel ranges).
            // And it might be a good idea to cache these.
            cv::Mat1b mask(labels == labelid);

            if(tbb::task::self().is_cancelled()) {
                //GGDBGM("aborted through tbb cancel." << endl);
                return;
            }

            // transform mask into icon
            cv::Mat1b masktrf = cv::Mat1b::zeros(iconSizecv);
            cv::warpAffine(mask, masktrf, trafo, iconSizecv, CV_INTER_AREA);

            if(tbb::task::self().is_cancelled()) {
                //GGDBGM("aborted through tbb cancel." << endl);
                return;
            }
            // The rest is probably too fast to allow checking for cancellation.

            QColor color = ctx.colors.at(labelid);

            // Fill icon with solid color in ARGB format.
            cv::Vec4b argb(0, color.red(), color.green(), color.blue());
            cv::Mat4b icon = cv::Mat4b(iconSizecv.height,
                                       iconSizecv.width,
                                       argb);

            // Now apply alpha channel.
            // Note: this is better than OpenCV's mask functionality as it
            // preserves the antialiasing!

            // Make ARGB 'array' which is interleaved to a true ARGB image
            // using mixChannels.
            const cv::Mat1b zero = cv::Mat1b::zeros(iconSizecv.height,
                                                    iconSizecv.width);
            const cv::Mat in[] = {masktrf, zero, zero, zero};
            // Copy only the alpha channel (0) of the in array into the
            // alpha channel (0) of the ARGB icon.
            const int mix[] = {0,0};
            // 4 input matrices, 1 dest, 1 mix-pair
            cv::mixChannels(in,4, &icon,1, mix,1);
            // convert the result to a QImage
            QImage qimage = Mat2QImage(icon);

            /* draw a border (alternative: the icon view could do this) */
            QPainter p(&qimage);
            QPen pen(color);
            // ensure border visibility, fixed to 1px
            pen.setWidthF(1.f);
            p.setPen(pen);
            p.drawRect(brect);

            ctx.icons[labelid] = qimage;
        }
    }
private:

    IconTaskCtx& ctx;
    //! The label matrix, either ctx.full_labels or ctx.roi_labels, depending
    //! on ctx.applyROI.
    cv::Mat1s labels;
    //! Icon size as cv::Size
    cv::Size iconSizecv;
    //! Icon inner size without border
    QSize innerSize;
    cv::Size innerSizecv;
    //! Scale factor from image to inner icon size
    float scale;
    //! icon offset in x-dir
    float dx;
    //! icon offset in y-dir
    float dy;
    //! Affine trafo using the above scale and offsets.
    cv::Mat1f trafo;
    //! Rect of the transformed mask.
    QRectF drect;
    //! Rect of the border drawn around the transformed mask.
    QRectF brect;
};

TaskLabelsIcons::TaskLabelsIcons(QSize size, bool applyROI)
    : Task("labels.icons", { {"source", {"labels"}} }),
      size(size), applyROI(applyROI)
{
}

bool TaskLabelsIcons::run()
{
    Subscription::Lock<Labels> source_lock(*sub("source"));
    Labels l = *source_lock();

    int nlabels = l.colors.size();
    IconTaskCtx ctx(nlabels, l.fullLabels, l.scopedlabels, size, applyROI,
                    l.colors);

    ctx.icons = QVector<QImage>(ctx.nlabels);
    tbb::auto_partitioner partitioner;

    tbb::parallel_for(tbb::blocked_range<short>(0, ctx.nlabels),
                      ComputeIconMasks(ctx), partitioner,
                      tbbTaskGroupContext);


    Subscription::Lock<QVector<QImage>> dest_lock(*sub("dest"));
    dest_lock.swap(ctx.icons);

    dest_lock.setVersion(source_lock.version());

    return true;

}


