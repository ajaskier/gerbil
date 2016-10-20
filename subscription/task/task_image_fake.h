#ifndef TASK_IMAGE_FAKE_H
#define TASK_IMAGE_FAKE_H

#include <QObject>
#include <task/task.h>

class TaskImageFAKE : public Task
{

public:
    explicit TaskImageFAKE(int a);
    virtual ~TaskImageFAKE();

    virtual bool run() override;

private:

    int a;

};

#endif // TASK_IMAGE_FAKE_H
