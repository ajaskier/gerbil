
#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>

#include "background_task/background_task_queue.h"
#include "background_task/background_task.h"
#include "background_task/background_executor.h"
#include "background_task/tasks/tbb/band2qimagetbb.h"


class Simulation : public QObject {
    Q_OBJECT

public:
    Simulation() {

        connect(this, SIGNAL(start()), this, SLOT(processStart()), Qt::QueuedConnection);
        emit start();

    }

signals:
    void start();

private slots:
    void processStart() {
        BackgroundExecutor executor;
        executor.run();

        multi_img image("/home/olek/gerbil_data/peppers_descriptor.txt");
        qimage_ptr dest(new SharedData<QImage>(new QImage()));
        qimage_ptr dest2(new SharedData<QImage>(new QImage()));
        SharedMultiImgPtr sharedImage(new SharedMultiImgBase(&image));

        BackgroundTaskPtr bt1_ptr(new Band2QImageTbb(sharedImage, dest, 1));
        connect(bt1_ptr.get(), SIGNAL(done(bool)), this, SLOT(bt1End(bool)), Qt::DirectConnection);
        executor.push(bt1_ptr);

        BackgroundTaskPtr bt2_ptr(new Band2QImageTbb(sharedImage, dest2, 20));
        connect(bt2_ptr.get(), SIGNAL(done(bool)), this, SLOT(bt2End(bool))), Qt::BlockingQueuedConnection;
        executor.push(bt2_ptr);




        while(true) {
            if(executor.isIdle()) {
                QImage result = **dest;
                result.save("/home/olek/output.png");

                QImage result2 = **dest2;
                result2.save("/home/olek/output2.png");

                break;
            } else {
             //  std::cout << "queue working! Still \n"; // << queue.size()
    //                      << " tasks in queue\n";
            }
        }
        std::cout << "done working" << std::endl;
    }

private slots:

    void bt1End(bool success) {
        std::cout << "First task done!\n";
    }

    void bt2End(bool success) {
        std::cout << "Second task done!\n";
    }

};

#endif
