#ifndef TASK_LABELS_ICONS_H
#define TASK_LABELS_ICONS_H

#include <QObject>
#include <QSize>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

#include <tbb/task_group.h>

struct IconTaskCtx {
    explicit IconTaskCtx(int nlabels,
                         const cv::Mat1s& full_labels,
                         const cv::Mat1s& roi_labels,
                         const QSize& iconSize,
                         bool applyROI,
                         const QVector<QColor>& colors)
            : nlabels(nlabels),
              full_labels(full_labels),
              roi_labels(roi_labels),
              iconSize(iconSize),
              applyROI(applyROI),
              colors(colors)
    {}

    // default copy constructor and assignment

    // Inputs:
    // number of labels (including background == colors.size)
    const int nlabels;
    const cv::Mat1s full_labels;
    const cv::Mat1s roi_labels;
    const QSize iconSize;
    const bool applyROI;
    const QVector<QColor> colors;

    // Result:
    QVector<QImage> icons;
};

class TaskLabelsIcons : public Task
{

public:
    explicit TaskLabelsIcons(QSize size, bool applyROI);
    virtual bool run() override;

    enum {IconSizeMin = 4};
    enum {IconSizeMax = 1024};

protected:

    QSize size;
    bool applyROI;
    tbb::task_group_context tbbTaskGroupContext;
   // IconTaskCtx ctx;
};


#endif // TASK_LABELS_ICONS_H
