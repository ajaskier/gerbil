#ifndef TASK_SET_LABELS_H
#define TASK_SET_LABELS_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskSetLabels : public Task
{

public:
    explicit TaskSetLabels(const Labeling &labeling, bool full);
    virtual ~TaskSetLabels();

    virtual bool run() override;

protected:

    Labels getLabels();
    void setColors(const std::vector<cv::Vec3b> &newColors, Labels &l);

    const Labeling &labeling;
    bool full;

};


#endif // TASK_SET_LABELS_H
