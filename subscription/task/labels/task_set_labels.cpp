#include "task_set_labels.h"

#include "subscription.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD 0.1

TaskSetLabels::TaskSetLabels(const Labeling &labeling, bool full)
    : Task("setLabels", "labels", { {"source", {"image.IMG"}}, {"ROI", {"ROI"}} }),
                      labeling(labeling), full(full)
{
}



bool TaskSetLabels::run()
{
    Labels l = getLabels(labeling());


    if (labeling.colors().size() < 2) {
        setColors(Labeling::colors(2, true), l);
    } else {
        setColors(labeling.colors(), l);
    }

    Subscription::Lock<Labels> dest_lock(*sub("dest"));
    dest_lock.swap(l);

    Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
    dest_lock.setVersion(roi_lock.version());

    return true;

}

Labels TaskSetLabels::getLabels(cv::Mat1s m)
{
    Labels l;
    if (DataConditionInformer::isInitialized("labels")) {
        Subscription::Lock<Labels> dest_lock(*sub("dest"));
        l = *dest_lock();

        Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
        cv::Rect roi = *roi_lock();

        if (roi_lock.version() > dest_lock.version()) {
            l.scopedlabels = cv::Mat1s(l.fullLabels, roi);
        } else {

            full = full ||
                    (m.cols == l.fullLabels.cols
                     && m.rows == l.fullLabels.rows);

            if (full) {
                m.copyTo(l.fullLabels);
            } else {
                m.copyTo(l.scopedlabels);
            }
        }

    } else {
        //initial
        Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
        cv::Rect roi = *roi_lock();

        l.fullLabels = cv::Mat1s(roi.height, roi.width, (short)0);
        l.scopedlabels = cv::Mat1s(l.fullLabels, roi);

    }

    return l;
}

void TaskSetLabels::setColors(const std::vector<cv::Vec3b> &newColors, Labels& l)
{
    QVector<QColor> col = Vec2QColor(newColors);
    col[0] = Qt::white; // override black for label 0

    l.colors.swap(col);

}
