#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <QThread>
#include <task/task.h>

class WorkerThread : public QThread
{
    Q_OBJECT

    void run() override {
        QString id = t->getId();
        auto success = t->start();
        //delete t;

        emit finished(id, success);
    }

public:
    WorkerThread(std::shared_ptr<Task> t, QObject* parent = 0) : QThread(parent), t(t) {}

signals:
    void finished(QString id, bool success);

private:
   std::shared_ptr<Task> t;
};

#endif // WORKER_THREAD_H
