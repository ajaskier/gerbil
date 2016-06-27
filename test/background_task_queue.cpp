#include <gmock/gmock.h>

#include <multi_img/multi_img.h>
#include <opencv2/highgui/highgui.hpp>
#include <QPixmap>

#include "background_task/background_task_queue.h"
#include "background_task/background_task.h"
#include "background_task/background_executor.h"
#include "background_task/tasks/tbb/band2qimagetbb.h"
#include <boost/thread.hpp>

//using BackgroundTaskPtr = BackgroundTaskPtrNew;
//using BackgroundTask = BackgroundTaskNew;
//using BackgroundTaskQueue = BackgroundTaskQueueNew;


#include <QApplication>

using namespace testing;

class BackgroundExecutorTest : public Test {

public:
    BackgroundExecutorTest() {

        int argc = 0;
        QApplication app(argc, nullptr);
        app.exec();
    }

//    BackgroundExecutor executor;

};

//TEST_F(BackgroundExecutorTest, DISABLED_isIdle_test) {

//    BackgroundTaskPtr bt1_ptr(new BackgroundTask());
//    BackgroundTaskPtr bt2_ptr(new BackgroundTask());

//    executor.push(bt1_ptr);
//    executor.push(bt2_ptr);

//    ASSERT_FALSE(executor.isIdle());

//}

//TEST_F(BackgroundExecutorTest, DISABLED_cancelTasks_test) {

//    BackgroundTaskPtr bt1_ptr(new BackgroundTask());
//    BackgroundTaskPtr bt2_ptr(new BackgroundTask());

//    executor.push(bt1_ptr);
//    executor.push(bt2_ptr);

//    ASSERT_FALSE(queue.isIdle());

//    executor.stop();

//    ASSERT_TRUE(executor.isIdle());

//}

//TEST_F(BackgroundExecutorTest, dummy_test) {



//    ASSERT_TRUE(5==(6-1));
//}

TEST_F(BackgroundExecutorTest, realTask_test) {

    BackgroundExecutor executor;
    executor.run();

    multi_img image("/home/olek/gerbil_data/peppers_descriptor.txt");
    qimage_ptr dest(new SharedData<QImage>(new QImage()));
    SharedMultiImgPtr sharedImage(new SharedMultiImgBase(&image));

    BackgroundTaskPtr bt1_ptr(new Band2QImageTbb(sharedImage, dest, 1));
    executor.push(bt1_ptr);

    ASSERT_FALSE(executor.isIdle());

    while(true) {
        if(executor.isIdle()) {
            QImage result = **dest;
            result.save("/home/olek/output.png");
            break;
        } else {
           std::cout << "queue working! Still \n"; // << queue.size()
//                      << " tasks in queue\n";
        }
    }
    std::cout << "done working" << std::endl;
    ASSERT_TRUE(executor.isIdle());

    //executor.kill();
    std::cout << "killed him" << std::endl;
    SUCCEED();

}

//TEST_F(BackgroundExecutorTest, DISABLED_multi_realTask_test) {

//    multi_img image("/home/olek/gerbil_data/peppers_descriptor.txt");
//    SharedMultiImgPtr sharedImage(new SharedMultiImgBase(&image));

//    std::vector<BackgroundTaskPtr> tasks;
//    for(int i = 0; i<image.size(); i++) {
//        qimage_ptr dest(new SharedData<QImage>(new QImage()));
//        tasks.push_back(BackgroundTaskPtr(new Band2QImageTbb(sharedImage, dest, i)));
//        queue.push(tasks[i]);
//        if(i != 0) tasks[i-1]->cancel();
//        if(i > 10) tasks[i-8]->cancel();
//        //if(i%2) {
//           // auto size = queue.size();
//            //std::cout << "cancelling " << size << " tasks\n";
//            //queue.cancelTasks();
//        //}
//    }

//    ASSERT_FALSE(queue.isIdle());

//    auto current_size = queue.size();
//    while(true) {
//        if(queue.isIdle()) {
//           // QImage result = **dest;
//           // result.save("/home/olek/output.png");
//            std::cout << "queue done working\n";
//            break;
//        } else {
//            if (queue.size() != current_size) {
//                current_size = queue.size();
//                std::cout << "queue working! Still " << queue.size()
//                          << " tasks in queue\n";
//            }
//        }
//    }

//    ASSERT_TRUE(queue.isIdle());

//    auto offset = image.size();
//    for(int i = 0; i<image.size(); i++) {
//        qimage_ptr dest(new SharedData<QImage>(new QImage()));
//        tasks.push_back(BackgroundTaskPtr(new Band2QImageTbb(sharedImage, dest, i)));
//        queue.push(tasks[offset+i]);
//        if(i != 0) tasks[offset+i-1]->cancel();
//        if(i > 10) tasks[offset+i-8]->cancel();
//        //if(i%2) {
//           // auto size = queue.size();
//            //std::cout << "cancelling " << size << " tasks\n";
//            //queue.cancelTasks();
//        //}
//    }

//    while(true) {
//        if(queue.isIdle()) {
//           // QImage result = **dest;
//           // result.save("/home/olek/output.png");
//            std::cout << "queue done working\n";
//            break;
//        } else {
//            if (queue.size() != current_size) {
//                current_size = queue.size();
//                std::cout << "queue working! Still " << queue.size()
//                          << " tasks in queue\n";
//            }
//        }
//    }

//    ASSERT_TRUE(queue.isIdle());


//}
