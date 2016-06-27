///*
//	Copyright(c) 2012 Petr Koupy <petr.koupy@gmail.com>

//	This file may be licensed under the terms of of the GNU General Public
//	License, version 3, as published by the Free Software Foundation. You can
//	find it here: http://www.gnu.org/licenses/gpl.html
//*/

//#ifdef WITH_BOOST_THREAD

////#define BACKGROUND_TASK_QUEUE_DEBUG

//#include "background_task_queue.h"
//#include <tbb/task_scheduler_init.h>
//#include <iostream>
//#include <iomanip>

//bool BackgroundTaskQueue::isIdle()
//{
//    std::unique_lock<std::mutex> lock(mutex);
//    return (taskQueue.empty()) && (!currentTask);
//}

//size_t BackgroundTaskQueue::size()
//{
//    std::unique_lock<std::mutex> lock(mutex);
//    return taskQueue.size();
//}

//void BackgroundTaskQueue::stop()
//{
//    std::unique_lock<std::mutex> lock(mutex);
//    stopped = true;
//    taskQueue.clear(); // Flush the queue, so there is nothing else to pop.
//    if (currentTask) {
//        cancelled = true;
//        currentTask->cancel();
//        currentTask.reset();
//    }
//    lock.unlock(); // Unlock to prevent deadlock when signalling the condition.
//    future.notify_all(); // In case the thread is sleeping.
//}

//void BackgroundTaskQueue::pop()
//{
//    std::unique_lock<std::mutex> lock(mutex);
//    while (taskQueue.empty()) {
//        if (stopped) {
//            return; // Thread will terminate.
//        } else {
//            future.wait(lock); // Yields lock until signalled.
//        }
//    }
//    currentTask = taskQueue.front(); // Fetch the task.

////    connect(currentTask.get(), SIGNAL(finished(bool)), this, SLOT(taskDone(bool)));
//    connect(currentTask.get(), SIGNAL(done(bool)), this, SLOT(taskDone(bool)));

//    taskQueue.pop_front();
//    cancelled = false;
//    currentTask->run();
//    working = true;
//    return;
//}

////void BackgroundTaskQueue::print()
////{
////	int row = 0;
////	for(std::deque<BackgroundTaskPtr>::const_iterator it = taskQueue.begin();
////		it != taskQueue.end();
////		++it, ++row)
////	{
////		std::string name = typeid(**it).name();
////		std::cout << std::setw(4) << row << " " << name << std::endl;
////	}
////}

//void BackgroundTaskQueue::push(BackgroundTaskPtr &task)
//{
//    // typedef boost::mutex Mutex;
//    // typedef boost::unique_lock<Mutex> Lock;
//    std::unique_lock<std::mutex> lock(mutex);
//    taskQueue.push_back(task);
//    lock.unlock(); // Unlock to prevent deadlock when signalling the condition.
//    future.notify_all();
//}

//void BackgroundTaskQueue::taskDone(bool success)
//{
//    std::unique_lock<std::mutex> lock(mutex);
//    currentTask.reset();
//    lock.unlock();

//    working = false;
//    pop();
//}

////void BackgroundTaskQueue::cancelTasks()
////{
////	Lock lock(mutex);
////	taskQueue.clear();
////	if (currentTask) {
////		cancelled = true;
////		currentTask->cancel();
////	}
////}

//void BackgroundTaskQueue::operator()()
//{
//    pop();
////    try {
////        // use for debugging, only one thread
////        // tbb::task_scheduler_init init(1);
////        while (true) {
////            if (stopped) {
////                break; // Thread termination.
////            }
////            if(!working) {
////                currentTask->run();
////                working = true;
////            }
////            //bool success = currentTask->run();
////           // {
////                //std::unique_lock<std::mutex> lock(mutex);
////                //currentTask->done(!cancelled && success);
////                //currentTask.reset();
////           // }
////        }
////    } catch (std::exception &) {
////        emit exception(std::current_exception(), true);
////    }
//}

//#endif
