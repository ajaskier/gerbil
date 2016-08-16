#ifndef TASK_PCA_TBB_H
#define TASK_PCA_TBB_H

#include <QObject>
#include <task/task.h>
#include <tbb/tbb.h>

#include <opencv2/core/core.hpp>

class TaskPcaTbb : public Task
{
    Q_OBJECT
public:
    explicit TaskPcaTbb(unsigned int components = 0, bool includecache = true,
                        QObject* parent = nullptr);
    virtual ~TaskPcaTbb();

    virtual void run() override;
    virtual void setSubscription(QString id, std::shared_ptr<Subscription> sub) override;

private:

    std::shared_ptr<Subscription> sourceSub;
    std::shared_ptr<Subscription> currentSub;
    tbb::task_group_context stopper;
    unsigned int components;
    bool includecache;
};


#endif // TASK_PCA_TBB_H
