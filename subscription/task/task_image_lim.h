#ifndef TASK_IMAGE_LIM_H
#define TASK_IMAGE_LIM_H

#include <QObject>
#include <task/task.h>

class TaskImageLim : public Task
{

public:
    explicit TaskImageLim(const QString &filename, bool limitedMode);
    virtual ~TaskImageLim();

    virtual bool run() override;

private:

    const QString& filename;
    bool limitedMode;
};


#endif // TASK_IMAGE_LIM_H
