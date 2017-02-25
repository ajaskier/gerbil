#include "task_image_rgb_tbb.h"
#include "subscription.h"
#include "lock.h"

#include <opencv2/core.hpp>

#include <QRgb>
#include <QImage>
#include <QString>

class Rgb {
public:
    Rgb(cv::Mat_<cv::Vec3f> &bgr, QImage &rgb)
        : bgr(bgr), rgb(rgb) {}
    void operator()(const tbb::blocked_range2d<int> &r) const
    {
        for (int y = r.rows().begin(); y != r.rows().end(); ++y) {
            cv::Vec3f *row = bgr[y];
            QRgb *destrow = (QRgb*)rgb.scanLine(y);
            for (int x = r.cols().begin(); x != r.cols().end(); ++x) {
                cv::Vec3f &c = row[x];
                destrow[x] = qRgb(c[2]*255., c[1]*255., c[0]*255.);
            }
        }
    }

private:
    cv::Mat_<cv::Vec3f> &bgr;
    QImage &rgb;
};

TaskImageRgbTbb::TaskImageRgbTbb(QString destId, QString sourceId)
    : Task(destId, {{"source", {sourceId}}})
{
}

bool TaskImageRgbTbb::run()
{

    Subscription::Lock<cv::Mat3f> source_lock(*sub("source"));
    cv::Mat3f& bgrmat = *source_lock();

    QImage *newRgb = new QImage(bgrmat.cols, bgrmat.rows, QImage::Format_ARGB32);

    Rgb computeRgb(bgrmat, *newRgb);
    tbb::parallel_for(tbb::blocked_range2d<int>(0, bgrmat.rows, 0, bgrmat.cols),
        computeRgb, tbb::auto_partitioner(), stopper);

    if (isCancelled()) {
        delete newRgb;
        return false;
    }

    Subscription::Lock<QImage> dest_lock(*sub("dest"));
    dest_lock.swap(*newRgb);
    dest_lock.setVersion(source_lock.version());

    return true;
}
