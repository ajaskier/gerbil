
#ifndef BACKGROUND_EXECUTOR_H
#define BACKGROUND_EXECUTOR_H


#include "background_task_queue.h"
#include "background_task/background_worker.h"

class BackgroundExecutor : public QObject {
    Q_OBJECT

public:
    BackgroundExecutor() : worker(new BackgroundWorker(*this, queue)) {}
    ~BackgroundExecutor() {
        std::cout << "BackgroundExecutor destructor" << std::endl;
    }

    void run() {

        QObject::connect(worker.get(), SIGNAL(taskDone(bool)),
                         this, SLOT(processTaskDone(bool)), Qt::DirectConnection);

        worker->start();
    }

    void push(const BackgroundTaskPtr& task) {
        queue.push(task);
    }

    BackgroundTaskPtr pop() {
        BackgroundTaskPtr task = queue.pop();
        processing = true;

        return task;
    }

    void cancelAll() {
        //queue.flush();

        //std::unique_lock<std::mutex> lock(mu);
        //currentTask->cancel();
    }

    bool isIdle() {
        return queue.size() == 0 && !processing;
    }

private slots:
    void processTaskDone(bool success) {
        std::cout << "in slot\n";
        processing = false;
    }


private:

    BackgroundTaskQueue queue;
    BackgroundWorkerPtr worker;
    bool processing = false;

};


#endif
