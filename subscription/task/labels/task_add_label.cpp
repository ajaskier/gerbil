#include "task_add_label.h"

#include "subscription.h"
#include "lock.h"
#include "data_condition_informer.h"

#include "data.h"
#include "QDebug"

#include "labeling.h"
#include "qtopencv.h"

#define REUSE_THRESHOLD 0.1

TaskAddLabel::TaskAddLabel()
    : Task("addLabel", "labels", { {"ROI", {"ROI"}} })
{
}

bool TaskAddLabel::run()
{
    Subscription::Lock<Labels> dest_lock(*sub("dest"));
    Labels l = *dest_lock();

    int labelcount = std::max(l.colors.count(), 1);
    labelcount++;

    setColors(Labeling::colors(labelcount, true), l);

    dest_lock.swap(l);
    Subscription::Lock<cv::Rect> roi_lock(*sub("ROI"));
    dest_lock.setVersion(roi_lock.version());

    return true;

}

void TaskAddLabel::setColors(const std::vector<cv::Vec3b> &newColors, Labels& l)
{
    QVector<QColor> col = Vec2QColor(newColors);
    col[0] = Qt::white; // override black for label 0

    l.colors.swap(col);

}
