#ifndef TASK_LABELS_ALTER_PIXELS_H
#define TASK_LABELS_ALTER_PIXELS_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskLabelsAlterPixels : public Task
{

public:
    explicit TaskLabelsAlterPixels(const cv::Mat1s &newLabels, const cv::Mat1b &mask);

    virtual bool run() override;

protected:

    const cv::Mat1s &newLabels;
    const cv::Mat1b &mask;

};


#endif // TASK_LABELS_ALTER_PIXELS_H
