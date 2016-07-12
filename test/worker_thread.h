#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <QThread>
#include <functional>

class WorkerThread : public QThread
{
    Q_OBJECT

    void run() override {
        fn();
        emit done();
    }

public:
    WorkerThread(std::function<void()> fn, QObject* parent = 0);

signals:
    void done();

private:
    std::function<void()> fn;
};

#endif // WORKER_THREAD_H
