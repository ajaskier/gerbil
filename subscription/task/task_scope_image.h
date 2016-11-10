#ifndef TASK_SCOPE_IMAGE_H
#define TASK_SCOPE_IMAGE_H

#include <QObject>
#include <task/task.h>
#include <opencv2/core/core.hpp>
#include "multi_img.h"

class TaskScopeImage : public Task
{

public:
    explicit TaskScopeImage();
    virtual ~TaskScopeImage();

    virtual bool run() override;

};


#endif // TASK_SCOPE_IMAGE_H
