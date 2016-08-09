#ifndef TASK_NORML2TBB_H
#define TASK_NORML2TBB_H

#include <QObject>
#include <task/task.h>

#include <tbb/tbb.h>

class TaskNormL2Tbb : public Task
{
    Q_OBJECT
public:
    explicit TaskNormL2Tbb(QObject* parent = nullptr);
    virtual ~TaskNormL2Tbb();

    virtual void run() override;

private:
    virtual void setSubscription(QString id, std::unique_ptr<Subscription> sub) override;

    std::unique_ptr<Subscription> imgNORMSub;
    std::unique_ptr<Subscription> imgIMGSub;
    tbb::task_group_context stopper;
};

#endif // TASK_NORML2TBB_H
