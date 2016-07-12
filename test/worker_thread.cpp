#include "worker_thread.h"

WorkerThread::WorkerThread(std::function<void()> fn, QObject* parent)
    : QThread(parent), fn(fn)
{

}
