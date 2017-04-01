#ifndef TASK_LABELS_CONSOLIDATE_H
#define TASK_LABELS_CONSOLIDATE_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskLabelsConsolidate : public Task
{

public:
    explicit TaskLabelsConsolidate(const cv::Size originalImageSize);

    virtual bool run() override;

private:
    const cv::Size originalImageSize;
};


#endif // TASK_LABELS_CONSOLIDATE_H
