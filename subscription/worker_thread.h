#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <QThread>
#include <task/task.h>

class WorkerThread : public QThread
{
    Q_OBJECT

    void run() override {
        auto success = t->start();
        delete t;

        emit finished(success);
    }

public:
    WorkerThread(Task* t, QObject* parent = 0) : QThread(parent), t(t) {}

signals:
    void finished(bool success);

private:
   Task* t;
};

#endif // WORKER_THREAD_H
