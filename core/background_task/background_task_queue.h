
#ifndef BACKGROUND_TASK_QUEUE_H
#define BACKGROUND_TASK_QUEUE_H

#include <deque>

#include "background_task/background_task.h"

class BackgroundTaskQueue : public QObject {
    Q_OBJECT

public:
    BackgroundTaskQueue() {}

    void push(const BackgroundTaskPtr& task) {
        std::unique_lock<std::mutex> lock(mu);
        tasks.push_front(task);

        lock.unlock();
        not_empty.notify_one();
    }

    //void push(BacgroundTaskPtr&& task);

    BackgroundTaskPtr pop() {
        std::unique_lock<std::mutex> lock(mu);
        not_empty.wait(lock, [this]() {
            return !tasks.empty();
        });
        BackgroundTaskPtr task = tasks.back();
        tasks.pop_back();

        return task;
    }

    void flush() {
        std::unique_lock<std::mutex> lock(mu);
        tasks.clear();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(mu);
        return tasks.size();
    }


private:

    std::deque<BackgroundTaskPtr> tasks;
    std::mutex mu;
    std::condition_variable not_empty;

};

#endif
