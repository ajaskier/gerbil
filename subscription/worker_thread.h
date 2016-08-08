#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <QThread>
#include <task/task.h>

class WorkerThread : public QThread
{
    Q_OBJECT

    void run() override {
        t->start();
    }

public:
    WorkerThread(Task* t, QObject* parent = 0) : QThread(parent), t(t) {}

private:
   Task* t;
};

#endif // WORKER_THREAD_H
