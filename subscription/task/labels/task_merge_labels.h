#ifndef TASK_MERGE_LABELS_H
#define TASK_MERGE_LABELS_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskMergeLabels : public Task
{

public:
    explicit TaskMergeLabels(const QVector<int> mlabels);

    virtual bool run() override;

protected:

    const QVector<int> mlabels;
};


#endif // TASK_MERGE_LABELS_H
