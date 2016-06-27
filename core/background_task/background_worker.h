
#ifndef BACKGROUND_WORKER_H
#define BACKGROUND_WORKER_H

#include <thread>
#include <QObject>

#include <iostream>

class BackgroundExecutor;

#include "background_task/background_task_queue.h"
//#include "background_task/background_executor.h"

class BackgroundWorker : public QObject {
    Q_OBJECT

public:

    BackgroundWorker(BackgroundExecutor& executor, BackgroundTaskQueue& queue) : executor(executor), queue(queue) {}
    ~BackgroundWorker() {
        running = false;
        thread.join();
    }

    void start() {
        running = true;
        thread = std::thread(&BackgroundWorker::run, this);
    }

signals:
    void taskDone(bool success);

private:
    void run();

    BackgroundExecutor& executor;
    BackgroundTaskQueue& queue;
    std::thread thread;
    std::atomic_bool running;

};

using BackgroundWorkerPtr = std::unique_ptr<BackgroundWorker>;

#endif
