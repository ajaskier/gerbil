#ifndef TASK_ADD_LABEL_H
#define TASK_ADD_LABEL_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskAddLabel : public Task
{

public:
    explicit TaskAddLabel();

    virtual bool run() override;

protected:

    void setColors(const std::vector<cv::Vec3b> &newColors, Labels &l);

};


#endif // TASK_ADD_LABEL_H
