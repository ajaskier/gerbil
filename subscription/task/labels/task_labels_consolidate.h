#ifndef TASK_LABELS_CONSOLIDATE_H
#define TASK_LABELS_CONSOLIDATE_H

#include <QObject>
#include <task/task.h>

#include "labeling.h"
#include "model/labels_model.h"

class TaskLabelsConsolidate : public Task
{

public:
    explicit TaskLabelsConsolidate();

    virtual bool run() override;

protected:

};


#endif // TASK_LABELS_CONSOLIDATE_H
